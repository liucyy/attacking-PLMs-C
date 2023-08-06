static void ape_unpack_mono(APEContext *ctx, int count)

{

    if (ctx->frameflags & APE_FRAMECODE_STEREO_SILENCE) {

        

        av_log(ctx->avctx, AV_LOG_DEBUG, "pure silence mono\n");

        return;

    }



    entropy_decode(ctx, count, 0);

    ape_apply_filters(ctx, ctx->decoded[0], NULL, count);



    

    predictor_decode_mono(ctx, count);



    

    if (ctx->channels == 2) {

        memcpy(ctx->decoded[1], ctx->decoded[0], count * sizeof(*ctx->decoded[1]));

    }

}