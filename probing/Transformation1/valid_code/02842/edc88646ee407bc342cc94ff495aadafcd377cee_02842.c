static int dirac_decode_data_unit(AVCodecContext *avctx, const uint8_t *buf, int size)

{

    DiracContext *s   = avctx->priv_data;

    DiracFrame *pic   = NULL;

    AVDiracSeqHeader *dsh;

    int ret, i;

    uint8_t parse_code;

    unsigned tmp;



    if (size < DATA_UNIT_HEADER_SIZE)

        return AVERROR_INVALIDDATA;



    parse_code = buf[4];



    init_get_bits(&s->gb, &buf[13], 8*(size - DATA_UNIT_HEADER_SIZE));



    if (parse_code == DIRAC_PCODE_SEQ_HEADER) {

        if (s->seen_sequence_header)

            return 0;



        

        ret = av_dirac_parse_sequence_header(&dsh, buf + DATA_UNIT_HEADER_SIZE, size - DATA_UNIT_HEADER_SIZE, avctx);

        if (ret < 0) {

            av_log(avctx, AV_LOG_ERROR, "error parsing sequence header");

            return ret;

        }



        ret = ff_set_dimensions(avctx, dsh->width, dsh->height);

        if (ret < 0) {

            av_freep(&dsh);

            return ret;

        }



        ff_set_sar(avctx, dsh->sample_aspect_ratio);

        avctx->pix_fmt         = dsh->pix_fmt;

        avctx->color_range     = dsh->color_range;

        avctx->color_trc       = dsh->color_trc;

        avctx->color_primaries = dsh->color_primaries;

        avctx->colorspace      = dsh->colorspace;

        avctx->profile         = dsh->profile;

        avctx->level           = dsh->level;

        avctx->framerate       = dsh->framerate;

        s->bit_depth           = dsh->bit_depth;

        s->version.major       = dsh->version.major;

        s->version.minor       = dsh->version.minor;

        s->seq                 = *dsh;

        av_freep(&dsh);



        s->pshift = s->bit_depth > 8;



        avcodec_get_chroma_sub_sample(avctx->pix_fmt, &s->chroma_x_shift, &s->chroma_y_shift);



        ret = alloc_sequence_buffers(s);

        if (ret < 0)

            return ret;



        s->seen_sequence_header = 1;

    } else if (parse_code == DIRAC_PCODE_END_SEQ) { 

        free_sequence_buffers(s);

        s->seen_sequence_header = 0;

    } else if (parse_code == DIRAC_PCODE_AUX) {

        if (buf[13] == 1) {     

            int ver[3];

            

            if (sscanf(buf+14, "Schroedinger %d.%d.%d", ver, ver+1, ver+2) == 3)

                if (ver[0] == 1 && ver[1] == 0 && ver[2] <= 7)

                    s->old_delta_quant = 1;

        }

    } else if (parse_code & 0x8) {  

        if (!s->seen_sequence_header) {

            av_log(avctx, AV_LOG_DEBUG, "Dropping frame without sequence header\n");

            return AVERROR_INVALIDDATA;

        }



        

        for (i = 0; i < MAX_FRAMES; i++)

            if (s->all_frames[i].avframe->data[0] == NULL)

                pic = &s->all_frames[i];

        if (!pic) {

            av_log(avctx, AV_LOG_ERROR, "framelist full\n");

            return AVERROR_INVALIDDATA;

        }



        av_frame_unref(pic->avframe);



        

        tmp            =  parse_code & 0x03;                   

        if (tmp > 2) {

            av_log(avctx, AV_LOG_ERROR, "num_refs of 3\n");

            return AVERROR_INVALIDDATA;

        }

        s->num_refs      = tmp;

        s->is_arith      = (parse_code & 0x48) == 0x08;          

        s->low_delay     = (parse_code & 0x88) == 0x88;          

        s->core_syntax   = (parse_code & 0x88) == 0x08;          

        s->ld_picture    = (parse_code & 0xF8) == 0xC8;          

        s->hq_picture    = (parse_code & 0xF8) == 0xE8;          

        s->dc_prediction = (parse_code & 0x28) == 0x08;          

        pic->reference   = (parse_code & 0x0C) == 0x0C;          

        pic->avframe->key_frame = s->num_refs == 0;              

        pic->avframe->pict_type = s->num_refs + 1;               



        

        if (s->version.minor == 2 && parse_code == 0x88)

            s->ld_picture = 1;



        if (s->low_delay && !(s->ld_picture || s->hq_picture) ) {

            av_log(avctx, AV_LOG_ERROR, "Invalid low delay flag\n");

            return AVERROR_INVALIDDATA;

        }



        if ((ret = get_buffer_with_edge(avctx, pic->avframe, (parse_code & 0x0C) == 0x0C ? AV_GET_BUFFER_FLAG_REF : 0)) < 0)

            return ret;

        s->current_picture = pic;

        s->plane[0].stride = pic->avframe->linesize[0];

        s->plane[1].stride = pic->avframe->linesize[1];

        s->plane[2].stride = pic->avframe->linesize[2];



        if (alloc_buffers(s, FFMAX3(FFABS(s->plane[0].stride), FFABS(s->plane[1].stride), FFABS(s->plane[2].stride))) < 0)

            return AVERROR(ENOMEM);



        

        ret = dirac_decode_picture_header(s);

        if (ret < 0)

            return ret;



        

        ret = dirac_decode_frame_internal(s);

        if (ret < 0)

            return ret;

    }

    return 0;

}