static void parse_presentation_segment(AVCodecContext *avctx,

                                       const uint8_t *buf, int buf_size)

{

    PGSSubContext *ctx = avctx->priv_data;



    int x, y;



    int w = bytestream_get_be16(&buf);

    int h = bytestream_get_be16(&buf);



    av_dlog(avctx, "Video Dimensions %dx%d\n",

            w, h);

    if (av_image_check_size(w, h, 0, avctx) >= 0)

        avcodec_set_dimensions(avctx, w, h);



    

    buf++;



    ctx->presentation.id_number = bytestream_get_be16(&buf);



    

    buf += 3;



    ctx->presentation.object_number = bytestream_get_byte(&buf);

    if (!ctx->presentation.object_number)

        return;



    

    buf += 4;



    x = bytestream_get_be16(&buf);

    y = bytestream_get_be16(&buf);



    



    av_dlog(avctx, "Subtitle Placement x=%d, y=%d\n", x, y);



    if (x > avctx->width || y > avctx->height) {

        av_log(avctx, AV_LOG_ERROR, "Subtitle out of video bounds. x = %d, y = %d, video width = %d, video height = %d.\n",

               x, y, avctx->width, avctx->height);

        x = 0; y = 0;

    }



    

    ctx->presentation.x = x;

    ctx->presentation.y = y;

}