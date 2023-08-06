static int theora_decode_header(AVCodecContext *avctx, GetBitContext gb)

{

    Vp3DecodeContext *s = avctx->priv_data;



    s->theora = get_bits_long(&gb, 24);

    av_log(avctx, AV_LOG_INFO, "Theora bitstream version %X\n", s->theora);



    

    

    if (s->theora < 0x030200)

    {

        s->flipped_image = 1;

        av_log(avctx, AV_LOG_DEBUG, "Old (<alpha3) Theora bitstream, flipped image\n");

    }



    s->width = get_bits(&gb, 16) << 4;

    s->height = get_bits(&gb, 16) << 4;



    if(avcodec_check_dimensions(avctx, s->width, s->height)){

        av_log(avctx, AV_LOG_ERROR, "Invalid dimensions (%dx%d)\n", s->width, s->height);

        s->width= s->height= 0;

        return -1;

    }



    if (s->theora >= 0x030400)

    {

        skip_bits(&gb, 32); 

        

        skip_bits(&gb, 32); 

        skip_bits(&gb, 4); 

        skip_bits(&gb, 32); 



        skip_bits(&gb, 24); 

        skip_bits(&gb, 24); 

    }

    else

    {

        skip_bits(&gb, 24); 

        skip_bits(&gb, 24); 

    }



    skip_bits(&gb, 8); 

    skip_bits(&gb, 8); 



    skip_bits(&gb, 32); 

    skip_bits(&gb, 32); 

    skip_bits(&gb, 24); 

    skip_bits(&gb, 24); 



    if (s->theora < 0x030200)

        skip_bits(&gb, 5); 

    skip_bits(&gb, 8); 

    if (s->theora >= 0x030400)

        skip_bits(&gb, 2); 

    skip_bits(&gb, 24); 



    skip_bits(&gb, 6); 



    if (s->theora >= 0x030200)

    {

        skip_bits(&gb, 5); 



        if (s->theora < 0x030400)

            skip_bits(&gb, 5); 

    }







    avctx->width = s->width;

    avctx->height = s->height;



    return 0;

}