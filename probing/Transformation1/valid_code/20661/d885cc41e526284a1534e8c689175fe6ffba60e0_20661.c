static int mjpeg_decode_app(MJpegDecodeContext *s)

{

    int len, id, i;



    len = get_bits(&s->gb, 16);

    if (len < 5)

        return AVERROR_INVALIDDATA;

    if (8 * len > get_bits_left(&s->gb))

        return AVERROR_INVALIDDATA;



    id   = get_bits_long(&s->gb, 32);

    id   = av_be2ne32(id);

    len -= 6;



    if (s->avctx->debug & FF_DEBUG_STARTCODE)

        av_log(s->avctx, AV_LOG_DEBUG, "APPx %8X\n", id);



    

    

    if (id == AV_RL32("AVI1")) {

        

            s->buggy_avid = 1;

        i = get_bits(&s->gb, 8); len--;

        av_log(s->avctx, AV_LOG_DEBUG, "polarity %d\n", i);

#if 0

        skip_bits(&s->gb, 8);

        skip_bits(&s->gb, 32);

        skip_bits(&s->gb, 32);

        len -= 10;

#endif

        goto out;

    }







    if (id == AV_RL32("JFIF")) {

        int t_w, t_h, v1, v2;

        skip_bits(&s->gb, 8); 

        v1 = get_bits(&s->gb, 8);

        v2 = get_bits(&s->gb, 8);

        skip_bits(&s->gb, 8);



        s->avctx->sample_aspect_ratio.num = get_bits(&s->gb, 16);

        s->avctx->sample_aspect_ratio.den = get_bits(&s->gb, 16);



        if (s->avctx->debug & FF_DEBUG_PICT_INFO)

            av_log(s->avctx, AV_LOG_INFO,

                   "mjpeg: JFIF header found (version: %x.%x) SAR=%d/%d\n",

                   v1, v2,

                   s->avctx->sample_aspect_ratio.num,

                   s->avctx->sample_aspect_ratio.den);



        t_w = get_bits(&s->gb, 8);

        t_h = get_bits(&s->gb, 8);

        if (t_w && t_h) {

            

            if (len -10 - (t_w * t_h * 3) > 0)

                len -= t_w * t_h * 3;

        }

        len -= 10;

        goto out;

    }



    if (id == AV_RL32("Adob") && (get_bits(&s->gb, 8) == 'e')) {

        if (s->avctx->debug & FF_DEBUG_PICT_INFO)

            av_log(s->avctx, AV_LOG_INFO, "mjpeg: Adobe header found\n");

        skip_bits(&s->gb, 16); 

        skip_bits(&s->gb, 16); 

        skip_bits(&s->gb, 16); 

        skip_bits(&s->gb,  8); 

        len -= 7;

        goto out;

    }



    if (id == AV_RL32("LJIF")) {

        if (s->avctx->debug & FF_DEBUG_PICT_INFO)

            av_log(s->avctx, AV_LOG_INFO,

                   "Pegasus lossless jpeg header found\n");

        skip_bits(&s->gb, 16); 

        skip_bits(&s->gb, 16); 

        skip_bits(&s->gb, 16); 

        skip_bits(&s->gb, 16); 

        switch (get_bits(&s->gb, 8)) {

        case 1:

            s->rgb         = 1;

            s->pegasus_rct = 0;

            break;

        case 2:

            s->rgb         = 1;

            s->pegasus_rct = 1;

            break;

        default:

            av_log(s->avctx, AV_LOG_ERROR, "unknown colorspace\n");

        }

        len -= 9;

        goto out;

    }



    

    if ((s->start_code == APP1) && (len > (0x28 - 8))) {

        id   = get_bits_long(&s->gb, 32);

        id   = av_be2ne32(id);

        len -= 4;

        

        if (id == AV_RL32("mjpg")) {

#if 0

            skip_bits(&s->gb, 32); 

            skip_bits(&s->gb, 32); 

            skip_bits(&s->gb, 32); 

            skip_bits(&s->gb, 32); 

            skip_bits(&s->gb, 32); 

            skip_bits(&s->gb, 32); 

            skip_bits(&s->gb, 32); 

            skip_bits(&s->gb, 32); 

#endif

            if (s->avctx->debug & FF_DEBUG_PICT_INFO)

                av_log(s->avctx, AV_LOG_INFO, "mjpeg: Apple MJPEG-A header found\n");

        }

    }



out:

    

    if (len < 0)

        av_log(s->avctx, AV_LOG_ERROR,

               "mjpeg: error, decode_app parser read over the end\n");

    while (--len > 0)

        skip_bits(&s->gb, 8);



    return 0;

}