static int h261_decode_picture_header(H261Context *h)

{

    MpegEncContext *const s = &h->s;

    int format, i;

    uint32_t startcode = 0;



    for (i = get_bits_left(&s->gb); i > 24; i -= 1) {

        startcode = ((startcode << 1) | get_bits(&s->gb, 1)) & 0x000FFFFF;



        if (startcode == 0x10)

            break;

    }



    if (startcode != 0x10) {

        av_log(s->avctx, AV_LOG_ERROR, "Bad picture start code\n");

        return -1;

    }



    

    i = get_bits(&s->gb, 5); 

    if (i < (s->picture_number & 31))

        i += 32;

    s->picture_number = (s->picture_number & ~31) + i;



    s->avctx->time_base      = (AVRational) { 1001, 30000 };



    

    skip_bits1(&s->gb); 

    skip_bits1(&s->gb); 

    skip_bits1(&s->gb); 



    format = get_bits1(&s->gb);



    

    if (format == 0) { 

        s->width     = 176;

        s->height    = 144;

        s->mb_width  = 11;

        s->mb_height = 9;

    } else { 

        s->width     = 352;

        s->height    = 288;

        s->mb_width  = 22;

        s->mb_height = 18;

    }



    s->mb_num = s->mb_width * s->mb_height;



    skip_bits1(&s->gb); 

    skip_bits1(&s->gb); 



    

    while (get_bits1(&s->gb) != 0)

        skip_bits(&s->gb, 8);



    

    s->pict_type = AV_PICTURE_TYPE_P;



    h->gob_number = 0;

    return 0;

}