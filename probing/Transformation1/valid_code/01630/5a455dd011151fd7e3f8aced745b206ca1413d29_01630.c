static int dirac_decode_data_unit(AVCodecContext *avctx, const uint8_t *buf, int size)

{

    DiracContext *s   = avctx->priv_data;

    DiracFrame *pic   = NULL;

    int ret, i, parse_code = buf[4];

    unsigned tmp;



    if (size < DATA_UNIT_HEADER_SIZE)

        return -1;



    init_get_bits(&s->gb, &buf[13], 8*(size - DATA_UNIT_HEADER_SIZE));



    if (parse_code == pc_seq_header) {

        if (s->seen_sequence_header)

            return 0;



        

        if (avpriv_dirac_parse_sequence_header(avctx, &s->gb, &s->source))

            return -1;



        avcodec_get_chroma_sub_sample(avctx->pix_fmt, &s->chroma_x_shift, &s->chroma_y_shift);



        if (alloc_sequence_buffers(s))

            return -1;



        s->seen_sequence_header = 1;

    } else if (parse_code == pc_eos) { 

        free_sequence_buffers(s);

        s->seen_sequence_header = 0;

    } else if (parse_code == pc_aux_data) {

        if (buf[13] == 1) {     

            int ver[3];

            

            if (sscanf(buf+14, "Schroedinger %d.%d.%d", ver, ver+1, ver+2) == 3)

                if (ver[0] == 1 && ver[1] == 0 && ver[2] <= 7)

                    s->old_delta_quant = 1;

        }

    } else if (parse_code & 0x8) {  

        if (!s->seen_sequence_header) {

            av_log(avctx, AV_LOG_DEBUG, "Dropping frame without sequence header\n");

            return -1;

        }



        

        for (i = 0; i < MAX_FRAMES; i++)

            if (s->all_frames[i].avframe->data[0] == NULL)

                pic = &s->all_frames[i];

        if (!pic) {

            av_log(avctx, AV_LOG_ERROR, "framelist full\n");

            return -1;

        }



        av_frame_unref(pic->avframe);



        

        tmp            =  parse_code & 0x03;                   

        if (tmp > 2) {

            av_log(avctx, AV_LOG_ERROR, "num_refs of 3\n");

            return -1;

        }

        s->num_refs    = tmp;

        s->is_arith    = (parse_code & 0x48) == 0x08;          

        s->low_delay   = (parse_code & 0x88) == 0x88;          

        pic->avframe->reference = (parse_code & 0x0C) == 0x0C;  

        pic->avframe->key_frame = s->num_refs == 0;             

        pic->avframe->pict_type = s->num_refs + 1;              



        if ((ret = get_buffer_with_edge(avctx, pic->avframe, (parse_code & 0x0C) == 0x0C ? AV_GET_BUFFER_FLAG_REF : 0)) < 0)

            return ret;

        s->current_picture = pic;

        s->plane[0].stride = pic->avframe->linesize[0];

        s->plane[1].stride = pic->avframe->linesize[1];

        s->plane[2].stride = pic->avframe->linesize[2];



        if (alloc_buffers(s, FFMAX3(FFABS(s->plane[0].stride), FFABS(s->plane[1].stride), FFABS(s->plane[2].stride))) < 0)

            return AVERROR(ENOMEM);



        

        if (dirac_decode_picture_header(s))

            return -1;



        

        if (dirac_decode_frame_internal(s))

            return -1;

    }

    return 0;

}