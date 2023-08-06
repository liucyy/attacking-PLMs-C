static int parse_picture_segment(AVCodecContext *avctx,

                                  const uint8_t *buf, int buf_size)

{

    PGSSubContext *ctx = avctx->priv_data;



    uint8_t sequence_desc;

    unsigned int rle_bitmap_len, width, height;



    if (buf_size <= 4)

        return -1;

    buf_size -= 4;



    

    buf += 3;



    

    sequence_desc = bytestream_get_byte(&buf);



    if (!(sequence_desc & 0x80)) {

        

        if (buf_size > ctx->picture.rle_remaining_len)

            return -1;



        memcpy(ctx->picture.rle + ctx->picture.rle_data_len, buf, buf_size);

        ctx->picture.rle_data_len += buf_size;

        ctx->picture.rle_remaining_len -= buf_size;



        return 0;

    }



    if (buf_size <= 7)

        return -1;

    buf_size -= 7;



    

    rle_bitmap_len = bytestream_get_be24(&buf) - 2*2;



    

    width  = bytestream_get_be16(&buf);

    height = bytestream_get_be16(&buf);



    

    if (avctx->width < width || avctx->height < height) {

        av_log(avctx, AV_LOG_ERROR, "Bitmap dimensions larger than video.\n");

        return -1;

    }



    ctx->picture.w = width;

    ctx->picture.h = height;



    av_fast_malloc(&ctx->picture.rle, &ctx->picture.rle_buffer_size, rle_bitmap_len);



    if (!ctx->picture.rle)

        return -1;



    memcpy(ctx->picture.rle, buf, buf_size);

    ctx->picture.rle_data_len = buf_size;

    ctx->picture.rle_remaining_len = rle_bitmap_len - buf_size;



    return 0;

}