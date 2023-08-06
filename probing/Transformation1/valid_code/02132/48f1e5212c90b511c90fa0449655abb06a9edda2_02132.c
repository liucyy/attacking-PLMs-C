static int wma_decode_superframe(AVCodecContext *avctx, void *data,

                                 int *got_frame_ptr, AVPacket *avpkt)

{

    const uint8_t *buf = avpkt->data;

    int buf_size = avpkt->size;

    WMACodecContext *s = avctx->priv_data;

    int nb_frames, bit_offset, i, pos, len, ret;

    uint8_t *q;

    int16_t *samples;



    tprintf(avctx, "***decode_superframe:\n");



    if(buf_size==0){

        s->last_superframe_len = 0;

        return 0;

    }

    if (buf_size < s->block_align)

        return 0;

    buf_size = s->block_align;



    init_get_bits(&s->gb, buf, buf_size*8);



    if (s->use_bit_reservoir) {

        

        skip_bits(&s->gb, 4); 

        nb_frames = get_bits(&s->gb, 4) - (s->last_superframe_len <= 0);

    } else {

        nb_frames = 1;

    }



    

    s->frame.nb_samples = nb_frames * s->frame_len;

    if ((ret = avctx->get_buffer(avctx, &s->frame)) < 0) {

        av_log(avctx, AV_LOG_ERROR, "get_buffer() failed\n");

        return ret;

    }

    samples = (int16_t *)s->frame.data[0];



    if (s->use_bit_reservoir) {

        bit_offset = get_bits(&s->gb, s->byte_offset_bits + 3);



        if (s->last_superframe_len > 0) {

            

            

            if ((s->last_superframe_len + ((bit_offset + 7) >> 3)) >

                MAX_CODED_SUPERFRAME_SIZE)

                goto fail;

            q = s->last_superframe + s->last_superframe_len;

            len = bit_offset;

            while (len > 7) {

                *q++ = (get_bits)(&s->gb, 8);

                len -= 8;

            }

            if (len > 0) {

                *q++ = (get_bits)(&s->gb, len) << (8 - len);

            }



            

            init_get_bits(&s->gb, s->last_superframe, MAX_CODED_SUPERFRAME_SIZE*8);

            

            if (s->last_bitoffset > 0)

                skip_bits(&s->gb, s->last_bitoffset);

            

            if (wma_decode_frame(s, samples) < 0)

                goto fail;

            samples += s->nb_channels * s->frame_len;

            nb_frames--;

        }



        

        pos = bit_offset + 4 + 4 + s->byte_offset_bits + 3;



        init_get_bits(&s->gb, buf + (pos >> 3), (MAX_CODED_SUPERFRAME_SIZE - (pos >> 3))*8);

        len = pos & 7;

        if (len > 0)

            skip_bits(&s->gb, len);



        s->reset_block_lengths = 1;

        for(i=0;i<nb_frames;i++) {

            if (wma_decode_frame(s, samples) < 0)

                goto fail;

            samples += s->nb_channels * s->frame_len;

        }



        

        pos = get_bits_count(&s->gb) + ((bit_offset + 4 + 4 + s->byte_offset_bits + 3) & ~7);

        s->last_bitoffset = pos & 7;

        pos >>= 3;

        len = buf_size - pos;

        if (len > MAX_CODED_SUPERFRAME_SIZE || len < 0) {

            av_log(s->avctx, AV_LOG_ERROR, "len %d invalid\n", len);

            goto fail;

        }

        s->last_superframe_len = len;

        memcpy(s->last_superframe, buf + pos, len);

    } else {

        

        if (wma_decode_frame(s, samples) < 0)

            goto fail;

        samples += s->nb_channels * s->frame_len;

    }







    *got_frame_ptr   = 1;

    *(AVFrame *)data = s->frame;



    return s->block_align;

 fail:

    

    s->last_superframe_len = 0;

    return -1;

}