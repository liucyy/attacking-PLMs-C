static const UID *mxf_get_mpeg2_codec_ul(AVCodecContext *avctx)

{

    int long_gop = avctx->gop_size > 1 || avctx->has_b_frames;



    if (avctx->profile == 4) { 

        if (avctx->level == 8) 

            return &mxf_mpeg2_codec_uls[0+long_gop];

        else if (avctx->level == 4) 

            return &mxf_mpeg2_codec_uls[4+long_gop];

        else if (avctx->level == 6) 

            return &mxf_mpeg2_codec_uls[8+long_gop];

    } else if (avctx->profile == 0) { 

        if (avctx->level == 5) 

            return &mxf_mpeg2_codec_uls[2+long_gop];

        else if (avctx->level == 2) 

            return &mxf_mpeg2_codec_uls[6+long_gop];

    }

    return NULL;

}