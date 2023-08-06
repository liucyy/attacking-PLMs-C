static inline CopyRet receive_frame(AVCodecContext *avctx,

                                    void *data, int *data_size,

                                    uint8_t second_field)

{

    BC_STATUS ret;

    BC_DTS_PROC_OUT output = {

        .PicInfo.width  = avctx->width,

        .PicInfo.height = avctx->height,

    };

    CHDContext *priv = avctx->priv_data;

    HANDLE dev       = priv->dev;



    *data_size = 0;



    

    ret = DtsProcOutputNoCopy(dev, OUTPUT_PROC_TIMEOUT, &output);

    if (ret == BC_STS_FMT_CHANGE) {

        av_log(avctx, AV_LOG_VERBOSE, "CrystalHD: Initial format change\n");

        avctx->width  = output.PicInfo.width;

        avctx->height = output.PicInfo.height;

        return RET_COPY_AGAIN;

    } else if (ret == BC_STS_SUCCESS) {

        int copy_ret = -1;

        if (output.PoutFlags & BC_POUT_FLAGS_PIB_VALID) {

            if (priv->last_picture == -1) {

                

                priv->last_picture = output.PicInfo.picture_number - 1;

            }



            if (avctx->codec->id == CODEC_ID_MPEG4 &&

                output.PicInfo.timeStamp == 0) {

                av_log(avctx, AV_LOG_VERBOSE,

                       "CrystalHD: Not returning packed frame twice.\n");

                priv->last_picture++;

                DtsReleaseOutputBuffs(dev, NULL, FALSE);

                return RET_COPY_AGAIN;

            }



            print_frame_info(priv, &output);



            if (priv->last_picture + 1 < output.PicInfo.picture_number) {

                av_log(avctx, AV_LOG_WARNING,

                       "CrystalHD: Picture Number discontinuity\n");

                

               priv->last_picture = output.PicInfo.picture_number - 1;

            }



            copy_ret = copy_frame(avctx, &output, data, data_size, second_field);

            if (*data_size > 0) {

                avctx->has_b_frames--;

                priv->last_picture++;

                av_log(avctx, AV_LOG_VERBOSE, "CrystalHD: Pipeline length: %u\n",

                       avctx->has_b_frames);

            }

        } else {

            

            av_log(avctx, AV_LOG_ERROR, "CrystalHD: ProcOutput succeeded with "

                                        "invalid PIB\n");

            avctx->has_b_frames--;

            copy_ret = RET_OK;

        }

        DtsReleaseOutputBuffs(dev, NULL, FALSE);



        return copy_ret;

    } else if (ret == BC_STS_BUSY) {

        return RET_COPY_AGAIN;

    } else {

        av_log(avctx, AV_LOG_ERROR, "CrystalHD: ProcOutput failed %d\n", ret);

        return RET_ERROR;

    }

}