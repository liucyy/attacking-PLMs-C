static int parse_picture_segment(AVCodecContext *avctx,

                                  const uint8_t *buf, int buf_size)

{

    PGSSubContext *ctx = avctx->priv_data;



    uint8_t sequence_desc;

    unsigned int rle_bitmap_len, width, height;

    uint16_t picture_id;



    if (buf_size <= 4)

        return -1;

    buf_size -= 4;



    picture_id = bytestream_get_be16(&buf);



    

    buf++;



    

    sequence_desc = bytestream_get_byte(&buf);



    if (!(sequence_desc & 0x80)) {

        

        if (buf_size > ctx->pictures[picture_id].rle_remaining_len)

            return -1;



        memcpy(ctx->pictures[picture_id].rle + ctx->pictures[picture_id].rle_data_len, buf, buf_size);

        ctx->pictures[picture_id].rle_data_len += buf_size;

        ctx->pictures[picture_id].rle_remaining_len -= buf_size;



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



    if (buf_size > rle_bitmap_len) {

        av_log(avctx, AV_LOG_ERROR, "too much RLE data\n");

        return AVERROR_INVALIDDATA;

    }



    ctx->pictures[picture_id].w = width;

    ctx->pictures[picture_id].h = height;



    av_fast_malloc(&ctx->pictures[picture_id].rle, &ctx->pictures[picture_id].rle_buffer_size, rle_bitmap_len);



    if (!ctx->pictures[picture_id].rle)

        return -1;



    memcpy(ctx->pictures[picture_id].rle, buf, buf_size);

    ctx->pictures[picture_id].rle_data_len      = buf_size;

    ctx->pictures[picture_id].rle_remaining_len = rle_bitmap_len - buf_size;



    return 0;

}