int ff_ivi_decode_frame(AVCodecContext *avctx, void *data, int *data_size,

                        AVPacket *avpkt)

{

    IVI45DecContext *ctx = avctx->priv_data;

    const uint8_t   *buf = avpkt->data;

    int             buf_size = avpkt->size;

    int             result, p, b;



    init_get_bits(&ctx->gb, buf, buf_size * 8);

    ctx->frame_data = buf;

    ctx->frame_size = buf_size;



    result = ctx->decode_pic_hdr(ctx, avctx);

    if (result) {

        av_log(avctx, AV_LOG_ERROR,

               "Error while decoding picture header: %d\n", result);

        return -1;

    }

    if (ctx->gop_invalid)

        return AVERROR_INVALIDDATA;



    if (ctx->gop_flags & IVI5_IS_PROTECTED) {

        av_log(avctx, AV_LOG_ERROR, "Password-protected clip!\n");

        return -1;

    }



    ctx->switch_buffers(ctx);



    



    if (ctx->is_nonnull_frame(ctx)) {

        for (p = 0; p < 3; p++) {

            for (b = 0; b < ctx->planes[p].num_bands; b++) {

                result = decode_band(ctx, p, &ctx->planes[p].bands[b], avctx);

                if (result) {

                    av_log(avctx, AV_LOG_ERROR,

                           "Error while decoding band: %d, plane: %d\n", b, p);

                    return -1;

                }

            }

        }

    }



    



    

    

    

    if (avctx->codec_id == AV_CODEC_ID_INDEO4 && ctx->frame_type == 0) {

        while (get_bits(&ctx->gb, 8)); 

        skip_bits_long(&ctx->gb, 64);  

        if (get_bits_left(&ctx->gb) > 18 && show_bits(&ctx->gb, 18) == 0x3FFF8)

            av_log(avctx, AV_LOG_ERROR, "Buffer contains IP frames!\n");

    }



    if (ctx->frame.data[0])

        avctx->release_buffer(avctx, &ctx->frame);



    ctx->frame.reference = 0;


    if ((result = avctx->get_buffer(avctx, &ctx->frame)) < 0) {

        av_log(avctx, AV_LOG_ERROR, "get_buffer() failed\n");

        return result;

    }



    if (ctx->is_scalable) {

        if (avctx->codec_id == AV_CODEC_ID_INDEO4)

            ff_ivi_recompose_haar(&ctx->planes[0], ctx->frame.data[0], ctx->frame.linesize[0], 4);

        else

            ff_ivi_recompose53   (&ctx->planes[0], ctx->frame.data[0], ctx->frame.linesize[0], 4);

    } else {

        ff_ivi_output_plane(&ctx->planes[0], ctx->frame.data[0], ctx->frame.linesize[0]);

    }



    ff_ivi_output_plane(&ctx->planes[2], ctx->frame.data[1], ctx->frame.linesize[1]);

    ff_ivi_output_plane(&ctx->planes[1], ctx->frame.data[2], ctx->frame.linesize[2]);



    *data_size = sizeof(AVFrame);

    *(AVFrame*)data = ctx->frame;



    return buf_size;

}