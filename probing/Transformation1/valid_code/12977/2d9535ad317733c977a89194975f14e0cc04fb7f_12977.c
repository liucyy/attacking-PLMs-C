void avcodec_align_dimensions2(AVCodecContext *s, int *width, int *height,

                               int linesize_align[AV_NUM_DATA_POINTERS])

{

    int i;

    int w_align= 1;

    int h_align= 1;



    switch(s->pix_fmt){

    case PIX_FMT_YUV420P:

    case PIX_FMT_YUYV422:

    case PIX_FMT_UYVY422:

    case PIX_FMT_YUV422P:

    case PIX_FMT_YUV440P:

    case PIX_FMT_YUV444P:

    case PIX_FMT_GBRP:

    case PIX_FMT_GRAY8:

    case PIX_FMT_GRAY16BE:

    case PIX_FMT_GRAY16LE:

    case PIX_FMT_YUVJ420P:

    case PIX_FMT_YUVJ422P:

    case PIX_FMT_YUVJ440P:

    case PIX_FMT_YUVJ444P:

    case PIX_FMT_YUVA420P:

    case PIX_FMT_YUV420P9LE:

    case PIX_FMT_YUV420P9BE:

    case PIX_FMT_YUV420P10LE:

    case PIX_FMT_YUV420P10BE:

    case PIX_FMT_YUV422P9LE:

    case PIX_FMT_YUV422P9BE:

    case PIX_FMT_YUV422P10LE:

    case PIX_FMT_YUV422P10BE:

    case PIX_FMT_YUV444P9LE:

    case PIX_FMT_YUV444P9BE:

    case PIX_FMT_YUV444P10LE:

    case PIX_FMT_YUV444P10BE:

    case PIX_FMT_GBRP9LE:

    case PIX_FMT_GBRP9BE:

    case PIX_FMT_GBRP10LE:

    case PIX_FMT_GBRP10BE:

        w_align = 16; 

        h_align = 16 * 2; 

        break;

    case PIX_FMT_YUV411P:

    case PIX_FMT_UYYVYY411:

        w_align=32;

        h_align=8;

        break;

    case PIX_FMT_YUV410P:

        if(s->codec_id == CODEC_ID_SVQ1){

            w_align=64;

            h_align=64;

        }

    case PIX_FMT_RGB555:

        if(s->codec_id == CODEC_ID_RPZA){

            w_align=4;

            h_align=4;

        }

    case PIX_FMT_PAL8:

    case PIX_FMT_BGR8:

    case PIX_FMT_RGB8:

        if(s->codec_id == CODEC_ID_SMC){

            w_align=4;

            h_align=4;

        }

        break;

    case PIX_FMT_BGR24:

        if((s->codec_id == CODEC_ID_MSZH) || (s->codec_id == CODEC_ID_ZLIB)){

            w_align=4;

            h_align=4;

        }

        break;

    default:

        w_align= 1;

        h_align= 1;

        break;

    }



    *width = FFALIGN(*width , w_align);

    *height= FFALIGN(*height, h_align);

    if(s->codec_id == CODEC_ID_H264 || s->lowres)

        *height+=2; 

                    



    for (i = 0; i < AV_NUM_DATA_POINTERS; i++)

        linesize_align[i] = STRIDE_ALIGN;









#if HAVE_MMX

    if(s->codec_id == CODEC_ID_SVQ1 || s->codec_id == CODEC_ID_VP5 ||

       s->codec_id == CODEC_ID_VP6 || s->codec_id == CODEC_ID_VP6F ||

       s->codec_id == CODEC_ID_VP6A) {

        for (i = 0; i < AV_NUM_DATA_POINTERS; i++)

            linesize_align[i] = 16;

    }

#endif

}