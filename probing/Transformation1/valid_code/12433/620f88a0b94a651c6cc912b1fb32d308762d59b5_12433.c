static int vaapi_encode_h264_write_extra_header(AVCodecContext *avctx,

                                                VAAPIEncodePicture *pic,

                                                int index, int *type,

                                                char *data, size_t *data_len)

{

    VAAPIEncodeContext      *ctx = avctx->priv_data;

    VAAPIEncodeH264Context *priv = ctx->priv_data;

    VAAPIEncodeH264Options  *opt = ctx->codec_options;

    CodedBitstreamFragment   *au = &priv->current_access_unit;

    int err, i;



    if (priv->sei_needed) {

        if (priv->aud_needed) {

            vaapi_encode_h264_add_nal(avctx, au, &priv->aud);

            priv->aud_needed = 0;

        }



        memset(&priv->sei, 0, sizeof(priv->sei));

        priv->sei.nal_unit_header.nal_unit_type = H264_NAL_SEI;



        i = 0;

        if (pic->encode_order == 0 && opt->sei & SEI_IDENTIFIER) {

            priv->sei.payload[i].payload_type = H264_SEI_TYPE_USER_DATA_UNREGISTERED;

            priv->sei.payload[i].payload.user_data_unregistered = priv->identifier;

            ++i;

        }

        if (opt->sei & SEI_TIMING) {

            if (pic->type == PICTURE_TYPE_IDR) {

                priv->sei.payload[i].payload_type = H264_SEI_TYPE_BUFFERING_PERIOD;

                priv->sei.payload[i].payload.buffering_period = priv->buffering_period;

                ++i;

            }

            priv->sei.payload[i].payload_type = H264_SEI_TYPE_PIC_TIMING;

            priv->sei.payload[i].payload.pic_timing = priv->pic_timing;

            ++i;

        }

        if (opt->sei & SEI_RECOVERY_POINT && pic->type == PICTURE_TYPE_I) {

            priv->sei.payload[i].payload_type = H264_SEI_TYPE_RECOVERY_POINT;

            priv->sei.payload[i].payload.recovery_point = priv->recovery_point;

            ++i;

        }



        priv->sei.payload_count = i;

        av_assert0(priv->sei.payload_count > 0);



        err = vaapi_encode_h264_add_nal(avctx, au, &priv->sei);

        if (err < 0)

            goto fail;

        priv->sei_needed = 0;



        err = vaapi_encode_h264_write_access_unit(avctx, data, data_len, au);

        if (err < 0)

            goto fail;



        ff_cbs_fragment_uninit(&priv->cbc, au);



        *type = VAEncPackedHeaderRawData;

        return 0;



#if !HAVE_VAAPI_1

    } else if (priv->sei_cbr_workaround_needed) {

        

        

        

        

        *data_len = 0;

        *type = VAEncPackedHeaderH264_SEI;

        priv->sei_cbr_workaround_needed = 0;

        return 0;

#endif



    } else {

        return AVERROR_EOF;

    }



fail:

    ff_cbs_fragment_uninit(&priv->cbc, au);

    return err;

}