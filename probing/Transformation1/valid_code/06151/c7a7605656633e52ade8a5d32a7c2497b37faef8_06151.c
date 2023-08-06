static int decode_packet(AVCodecContext *avctx, void *data,

                         int *got_frame_ptr, AVPacket* avpkt)

{

    WMAProDecodeCtx *s = avctx->priv_data;

    GetBitContext* gb  = &s->pgb;

    const uint8_t* buf = avpkt->data;

    int buf_size       = avpkt->size;

    int num_bits_prev_frame;

    int packet_sequence_number;



    *got_frame_ptr = 0;



    if (s->packet_done || s->packet_loss) {

        s->packet_done = 0;



        

        if (buf_size < avctx->block_align) {

            av_log(avctx, AV_LOG_ERROR, "Input packet too small (%d < %d)\n",

                   buf_size, avctx->block_align);

            return AVERROR_INVALIDDATA;

        }



        s->next_packet_start = buf_size - avctx->block_align;

        buf_size = avctx->block_align;

        s->buf_bit_size = buf_size << 3;



        

        init_get_bits(gb, buf, s->buf_bit_size);

        packet_sequence_number = get_bits(gb, 4);

        skip_bits(gb, 2);



        

        num_bits_prev_frame = get_bits(gb, s->log2_frame_size);

        av_dlog(avctx, "packet[%d]: nbpf %x\n", avctx->frame_number,

                num_bits_prev_frame);



        

        if (!s->packet_loss &&

            ((s->packet_sequence_number + 1) & 0xF) != packet_sequence_number) {

            s->packet_loss = 1;

            av_log(avctx, AV_LOG_ERROR, "Packet loss detected! seq %x vs %x\n",

                   s->packet_sequence_number, packet_sequence_number);

        }

        s->packet_sequence_number = packet_sequence_number;



        if (num_bits_prev_frame > 0) {

            int remaining_packet_bits = s->buf_bit_size - get_bits_count(gb);

            if (num_bits_prev_frame >= remaining_packet_bits) {

                num_bits_prev_frame = remaining_packet_bits;

                s->packet_done = 1;

            }



            

            save_bits(s, gb, num_bits_prev_frame, 1);

            av_dlog(avctx, "accumulated %x bits of frame data\n",

                    s->num_saved_bits - s->frame_offset);



            

            if (!s->packet_loss)

                decode_frame(s, data, got_frame_ptr);

        } else if (s->num_saved_bits - s->frame_offset) {

            av_dlog(avctx, "ignoring %x previously saved bits\n",

                    s->num_saved_bits - s->frame_offset);

        }



        if (s->packet_loss) {

            

            s->num_saved_bits = 0;

            s->packet_loss = 0;

        }



    } else {

        int frame_size;

        s->buf_bit_size = (avpkt->size - s->next_packet_start) << 3;

        init_get_bits(gb, avpkt->data, s->buf_bit_size);

        skip_bits(gb, s->packet_offset);

        if (s->len_prefix && remaining_bits(s, gb) > s->log2_frame_size &&

            (frame_size = show_bits(gb, s->log2_frame_size)) &&

            frame_size <= remaining_bits(s, gb)) {

            save_bits(s, gb, frame_size, 0);

            s->packet_done = !decode_frame(s, data, got_frame_ptr);

        } else if (!s->len_prefix

                   && s->num_saved_bits > get_bits_count(&s->gb)) {

            

            s->packet_done = !decode_frame(s, data, got_frame_ptr);

        } else

            s->packet_done = 1;

    }



    if (s->packet_done && !s->packet_loss &&

        remaining_bits(s, gb) > 0) {

        

        save_bits(s, gb, remaining_bits(s, gb), 0);

    }



    s->packet_offset = get_bits_count(gb) & 7;

    if (s->packet_loss)

        return AVERROR_INVALIDDATA;



    return get_bits_count(gb) >> 3;

}