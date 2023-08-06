static void run_postproc(AVCodecContext *avctx, AVFrame *frame)

{

    DDSContext *ctx = avctx->priv_data;

    int i, x_off;



    switch (ctx->postproc) {

    case DDS_ALPHA_EXP:

        

        av_log(avctx, AV_LOG_DEBUG, "Post-processing alpha exponent.\n");



        for (i = 0; i < frame->linesize[0] * frame->height; i += 4) {

            uint8_t *src = frame->data[0] + i;

            int r = src[0];

            int g = src[1];

            int b = src[2];

            int a = src[3];



            src[0] = r * a / 255;

            src[1] = g * a / 255;

            src[2] = b * a / 255;

            src[3] = 255;

        }

        break;

    case DDS_NORMAL_MAP:

        

        av_log(avctx, AV_LOG_DEBUG, "Post-processing normal map.\n");



        x_off = ctx->tex_ratio == 8 ? 0 : 3;

        for (i = 0; i < frame->linesize[0] * frame->height; i += 4) {

            uint8_t *src = frame->data[0] + i;

            int x = src[x_off];

            int y = src[1];

            int z = 127;



            int d = (255 * 255 - x * x - y * y) / 2;

            if (d > 0)

                z = rint(sqrtf(d));



            src[0] = x;

            src[1] = y;

            src[2] = z;

            src[3] = 255;

        }

        break;

    case DDS_RAW_YCOCG:

        

        av_log(avctx, AV_LOG_DEBUG, "Post-processing raw YCoCg.\n");



        for (i = 0; i < frame->linesize[0] * frame->height; i += 4) {

            uint8_t *src = frame->data[0] + i;

            int a  = src[0];

            int cg = src[1] - 128;

            int co = src[2] - 128;

            int y  = src[3];



            src[0] = av_clip_uint8(y + co - cg);

            src[1] = av_clip_uint8(y + cg);

            src[2] = av_clip_uint8(y - co - cg);

            src[3] = a;

        }

        break;

    case DDS_SWAP_ALPHA:

        

        av_log(avctx, AV_LOG_DEBUG, "Post-processing swapped Luma/Alpha.\n");



        for (i = 0; i < frame->linesize[0] * frame->height; i += 2) {

            uint8_t *src = frame->data[0] + i;

            FFSWAP(uint8_t, src[0], src[1]);

        }

        break;

    case DDS_SWIZZLE_A2XY:

        

        av_log(avctx, AV_LOG_DEBUG, "Post-processing A2XY swizzle.\n");

        do_swizzle(frame, 0, 1);

        break;

    case DDS_SWIZZLE_RBXG:

        

        av_log(avctx, AV_LOG_DEBUG, "Post-processing RBXG swizzle.\n");

        do_swizzle(frame, 1, 3);

        do_swizzle(frame, 2, 3);

        break;

    case DDS_SWIZZLE_RGXB:

        

        av_log(avctx, AV_LOG_DEBUG, "Post-processing RGXB swizzle.\n");

        do_swizzle(frame, 2, 3);

        break;

    case DDS_SWIZZLE_RXBG:

        

        av_log(avctx, AV_LOG_DEBUG, "Post-processing RXBG swizzle.\n");

        do_swizzle(frame, 1, 3);

        break;

    case DDS_SWIZZLE_RXGB:

        

        av_log(avctx, AV_LOG_DEBUG, "Post-processing RXGB swizzle.\n");

        do_swizzle(frame, 0, 3);

        break;

    case DDS_SWIZZLE_XGBR:

        

        av_log(avctx, AV_LOG_DEBUG, "Post-processing XGBR swizzle.\n");

        do_swizzle(frame, 2, 3);

        do_swizzle(frame, 0, 3);

        break;

    case DDS_SWIZZLE_XGXR:

        

        av_log(avctx, AV_LOG_DEBUG, "Post-processing XGXR swizzle.\n");

        do_swizzle(frame, 1, 3);

        do_swizzle(frame, 0, 3);

        do_swizzle(frame, 0, 1);

        break;

    case DDS_SWIZZLE_XRBG:

        

        av_log(avctx, AV_LOG_DEBUG, "Post-processing XRBG swizzle.\n");

        do_swizzle(frame, 1, 3);

        do_swizzle(frame, 0, 3);

        break;

    }

}