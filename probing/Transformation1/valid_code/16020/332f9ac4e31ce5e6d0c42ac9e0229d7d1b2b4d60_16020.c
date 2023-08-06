int intel_h263_decode_picture_header(MpegEncContext *s)

{

    int format;



    

    if (get_bits_long(&s->gb, 22) != 0x20) {

        av_log(s->avctx, AV_LOG_ERROR, "Bad picture start code\n");

        return -1;

    }

    s->picture_number = get_bits(&s->gb, 8); 



    if (get_bits1(&s->gb) != 1) {

        av_log(s->avctx, AV_LOG_ERROR, "Bad marker\n");

        return -1;	

    }

    if (get_bits1(&s->gb) != 0) {

        av_log(s->avctx, AV_LOG_ERROR, "Bad H263 id\n");

        return -1;	

    }

    skip_bits1(&s->gb);	

    skip_bits1(&s->gb);	

    skip_bits1(&s->gb);	



    format = get_bits(&s->gb, 3);

    if (format != 7) {

        av_log(s->avctx, AV_LOG_ERROR, "Intel H263 free format not supported\n");

        return -1;

    }

    s->h263_plus = 0;



    s->pict_type = I_TYPE + get_bits1(&s->gb);

    

    s->unrestricted_mv = get_bits1(&s->gb); 

    s->h263_long_vectors = s->unrestricted_mv;



    if (get_bits1(&s->gb) != 0) {

        av_log(s->avctx, AV_LOG_ERROR, "SAC not supported\n");

        return -1;	

    }

    if (get_bits1(&s->gb) != 0) {

        s->obmc= 1;

        av_log(s->avctx, AV_LOG_ERROR, "Advanced Prediction Mode not supported\n");



    }

    if (get_bits1(&s->gb) != 0) {

        av_log(s->avctx, AV_LOG_ERROR, "PB frame mode no supported\n");

        return -1;	

    }



    

    skip_bits(&s->gb, 41);



    s->qscale = get_bits(&s->gb, 5);

    skip_bits1(&s->gb);	



    

    while (get_bits1(&s->gb) != 0) {

        skip_bits(&s->gb, 8);

    }

    s->f_code = 1;



    s->y_dc_scale_table=

    s->c_dc_scale_table= ff_mpeg1_dc_scale_table;



    return 0;

}