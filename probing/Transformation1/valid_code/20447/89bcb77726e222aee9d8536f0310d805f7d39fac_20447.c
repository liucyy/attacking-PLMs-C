static int parse_presentation_segment(AVCodecContext *avctx,

                                      const uint8_t *buf, int buf_size,

                                      int64_t pts)

{

    PGSSubContext *ctx = avctx->priv_data;

    int i, state, ret;




    

    int w = bytestream_get_be16(&buf);

    int h = bytestream_get_be16(&buf);



    uint16_t object_index;



    ctx->presentation.pts = pts;



    av_dlog(avctx, "Video Dimensions %dx%d\n",

            w, h);

    ret = ff_set_dimensions(avctx, w, h);

    if (ret < 0)

        return ret;



    

    buf++;



    

    ctx->presentation.id_number = bytestream_get_be16(&buf);

    

    state = bytestream_get_byte(&buf) >> 6;

    if (state != 0) {

        flush_cache(avctx);

    }



    

    buf += 1;

    ctx->presentation.palette_id = bytestream_get_byte(&buf);

    ctx->presentation.object_count = bytestream_get_byte(&buf);

    if (ctx->presentation.object_count > MAX_OBJECT_REFS) {

        av_log(avctx, AV_LOG_ERROR,

               "Invalid number of presentation objects %d\n",

               ctx->presentation.object_count);

        ctx->presentation.object_count = 2;

        if (avctx->err_recognition & AV_EF_EXPLODE) {

            return AVERROR_INVALIDDATA;

        }

    }





    for (i = 0; i < ctx->presentation.object_count; i++)

    {



        if (buf_end - buf < 8) {

            av_log(avctx, AV_LOG_ERROR, "Insufficent space for object\n");

            ctx->presentation.object_count = i;

            return AVERROR_INVALIDDATA;

        }



        ctx->presentation.objects[i].id = bytestream_get_be16(&buf);

        ctx->presentation.objects[i].window_id = bytestream_get_byte(&buf);

        ctx->presentation.objects[i].composition_flag = bytestream_get_byte(&buf);



        ctx->presentation.objects[i].x = bytestream_get_be16(&buf);

        ctx->presentation.objects[i].y = bytestream_get_be16(&buf);



        

        if (ctx->presentation.objects[i].composition_flag & 0x80) {

            ctx->presentation.objects[i].crop_x = bytestream_get_be16(&buf);

            ctx->presentation.objects[i].crop_y = bytestream_get_be16(&buf);

            ctx->presentation.objects[i].crop_w = bytestream_get_be16(&buf);

            ctx->presentation.objects[i].crop_h = bytestream_get_be16(&buf);

        }



        av_dlog(avctx, "Subtitle Placement x=%d, y=%d\n",

                ctx->presentation.objects[i].x, ctx->presentation.objects[i].y);



        if (ctx->presentation.objects[i].x > avctx->width ||

            ctx->presentation.objects[i].y > avctx->height) {

            av_log(avctx, AV_LOG_ERROR, "Subtitle out of video bounds. x = %d, y = %d, video width = %d, video height = %d.\n",

                   ctx->presentation.objects[i].x,

                   ctx->presentation.objects[i].y,

                    avctx->width, avctx->height);

            ctx->presentation.objects[i].x = 0;

            ctx->presentation.objects[i].y = 0;

            if (avctx->err_recognition & AV_EF_EXPLODE) {

                return AVERROR_INVALIDDATA;

            }

        }

    }



    return 0;

}