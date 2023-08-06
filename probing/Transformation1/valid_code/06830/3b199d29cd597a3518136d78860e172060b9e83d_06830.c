static av_cold int flic_decode_init(AVCodecContext *avctx)

{

    FlicDecodeContext *s = avctx->priv_data;

    unsigned char *fli_header = (unsigned char *)avctx->extradata;

    int depth;



    if (avctx->extradata_size != 12 &&

        avctx->extradata_size != 128) {

        av_log(avctx, AV_LOG_ERROR, "Expected extradata of 12 or 128 bytes\n");

        return AVERROR_INVALIDDATA;

    }



    s->avctx = avctx;



    s->fli_type = AV_RL16(&fli_header[4]); 



    depth = 0;

    if (s->avctx->extradata_size == 12) {

        

        s->fli_type = FLC_MAGIC_CARPET_SYNTHETIC_TYPE_CODE;

        depth = 8;

    } else {

        depth = AV_RL16(&fli_header[12]);

    }



    if (depth == 0) {

        depth = 8; 

    }



    if ((s->fli_type == FLC_FLX_TYPE_CODE) && (depth == 16)) {

        depth = 15; 

    }



    switch (depth) {

        case 8  : avctx->pix_fmt = AV_PIX_FMT_PAL8; break;

        case 15 : avctx->pix_fmt = AV_PIX_FMT_RGB555; break;

        case 16 : avctx->pix_fmt = AV_PIX_FMT_RGB565; break;

        case 24 : avctx->pix_fmt = AV_PIX_FMT_BGR24; 

                  av_log(avctx, AV_LOG_ERROR, "24Bpp FLC/FLX is unsupported due to no test files.\n");

                  return AVERROR_PATCHWELCOME;

        default :

                  av_log(avctx, AV_LOG_ERROR, "Unknown FLC/FLX depth of %d Bpp is unsupported.\n",depth);

                  return AVERROR_INVALIDDATA;

    }



    s->frame.data[0] = NULL;

    s->new_palette = 0;



    return 0;

}