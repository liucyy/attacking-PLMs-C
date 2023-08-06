static inline CopyRet copy_frame(AVCodecContext *avctx,

                                 BC_DTS_PROC_OUT *output,

                                 void *data, int *data_size)

{

    BC_STATUS ret;

    BC_DTS_STATUS decoder_status = { 0, };

    uint8_t trust_interlaced;

    uint8_t interlaced;



    CHDContext *priv = avctx->priv_data;

    int64_t pkt_pts  = AV_NOPTS_VALUE;

    uint8_t pic_type = 0;



    uint8_t bottom_field = (output->PicInfo.flags & VDEC_FLAG_BOTTOMFIELD) ==

                           VDEC_FLAG_BOTTOMFIELD;

    uint8_t bottom_first = !!(output->PicInfo.flags & VDEC_FLAG_BOTTOM_FIRST);



    int width    = output->PicInfo.width;

    int height   = output->PicInfo.height;

    int bwidth;

    uint8_t *src = output->Ybuff;

    int sStride;

    uint8_t *dst;

    int dStride;



    if (output->PicInfo.timeStamp != 0) {

        OpaqueList *node = opaque_list_pop(priv, output->PicInfo.timeStamp);

        if (node) {

            pkt_pts = node->reordered_opaque;

            pic_type = node->pic_type;

            av_free(node);

        } else {

            

            pic_type = PICT_BOTTOM_FIELD;

        }

        av_log(avctx, AV_LOG_VERBOSE, "output \"pts\": %"PRIu64"\n",

               output->PicInfo.timeStamp);

        av_log(avctx, AV_LOG_VERBOSE, "output picture type %d\n",

               pic_type);

    }



    ret = DtsGetDriverStatus(priv->dev, &decoder_status);

    if (ret != BC_STS_SUCCESS) {

        av_log(avctx, AV_LOG_ERROR,

               "CrystalHD: GetDriverStatus failed: %u\n", ret);

       return RET_ERROR;

    }



    

    trust_interlaced = avctx->codec->id != CODEC_ID_H264 ||

                       !(output->PicInfo.flags & VDEC_FLAG_UNKNOWN_SRC) ||

                       priv->need_second_field ||

                       (decoder_status.picNumFlags & ~0x40000000) ==

                       output->PicInfo.picture_number;



    

    if (output->PicInfo.picture_number == priv->last_picture && !priv->need_second_field) {

        av_log(avctx, AV_LOG_WARNING,

               "Incorrectly guessed progressive frame. Discarding second field\n");

        

        return RET_OK;

    }



    interlaced = (output->PicInfo.flags & VDEC_FLAG_INTERLACED_SRC) &&

                 trust_interlaced;



    if (!trust_interlaced && (decoder_status.picNumFlags & ~0x40000000) == 0) {

        av_log(avctx, AV_LOG_VERBOSE,

               "Next picture number unknown. Assuming progressive frame.\n");

    }



    av_log(avctx, AV_LOG_VERBOSE, "Interlaced state: %d | trust_interlaced %d\n",

           interlaced, trust_interlaced);



    if (priv->pic.data[0] && !priv->need_second_field)

        avctx->release_buffer(avctx, &priv->pic);



    priv->need_second_field = interlaced && !priv->need_second_field;



    priv->pic.buffer_hints = FF_BUFFER_HINTS_VALID | FF_BUFFER_HINTS_PRESERVE |

                             FF_BUFFER_HINTS_REUSABLE;

    if (!priv->pic.data[0]) {

        if (avctx->get_buffer(avctx, &priv->pic) < 0) {

            av_log(avctx, AV_LOG_ERROR, "get_buffer() failed\n");

            return RET_ERROR;

        }

    }



    bwidth = av_image_get_linesize(avctx->pix_fmt, width, 0);

    if (priv->is_70012) {

        int pStride;



        if (width <= 720)

            pStride = 720;

        else if (width <= 1280)

            pStride = 1280;

        else pStride = 1920;

        sStride = av_image_get_linesize(avctx->pix_fmt, pStride, 0);

    } else {

        sStride = bwidth;

    }



    dStride = priv->pic.linesize[0];

    dst     = priv->pic.data[0];



    av_log(priv->avctx, AV_LOG_VERBOSE, "CrystalHD: Copying out frame\n");



    if (interlaced) {

        int dY = 0;

        int sY = 0;



        height /= 2;

        if (bottom_field) {

            av_log(priv->avctx, AV_LOG_VERBOSE, "Interlaced: bottom field\n");

            dY = 1;

        } else {

            av_log(priv->avctx, AV_LOG_VERBOSE, "Interlaced: top field\n");

            dY = 0;

        }



        for (sY = 0; sY < height; dY++, sY++) {

            memcpy(&(dst[dY * dStride]), &(src[sY * sStride]), bwidth);

            dY++;

        }

    } else {

        av_image_copy_plane(dst, dStride, src, sStride, bwidth, height);

    }



    priv->pic.interlaced_frame = interlaced;

    if (interlaced)

        priv->pic.top_field_first = !bottom_first;



    priv->pic.pkt_pts = pkt_pts;



    if (!priv->need_second_field) {

        *data_size       = sizeof(AVFrame);

        *(AVFrame *)data = priv->pic;

    }



    

    if (!interlaced && (output->PicInfo.flags & VDEC_FLAG_UNKNOWN_SRC) &&

        (pic_type == PICT_TOP_FIELD || pic_type == PICT_BOTTOM_FIELD)) {

        av_log(priv->avctx, AV_LOG_VERBOSE, "Fieldpair from two packets.\n");

        return RET_SKIP_NEXT_COPY;

    }



    

    return priv->need_second_field &&

           !(output->PicInfo.flags & VDEC_FLAG_UNKNOWN_SRC) ?

           RET_COPY_NEXT_FIELD : RET_OK;

}