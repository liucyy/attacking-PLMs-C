static int parse_object_segment(AVCodecContext *avctx,

                                  const uint8_t *buf, int buf_size)

{

    PGSSubContext *ctx = avctx->priv_data;

    PGSSubObject *object;



    uint8_t sequence_desc;

    unsigned int rle_bitmap_len, width, height;

    int id;



    if (buf_size <= 4)

        return AVERROR_INVALIDDATA;

    buf_size -= 4;



    id = bytestream_get_be16(&buf);

    object = find_object(id, &ctx->objects);

    if (!object) {

        if (ctx->objects.count >= MAX_EPOCH_OBJECTS) {

            av_log(avctx, AV_LOG_ERROR, "Too many objects in epoch\n");

            return AVERROR_INVALIDDATA;

        }

        object = &ctx->objects.object[ctx->objects.count++];

        object->id = id;

    }



    

    buf += 1;



    

    sequence_desc = bytestream_get_byte(&buf);



    if (!(sequence_desc & 0x80)) {

        

        if (buf_size > object->rle_remaining_len)

            return AVERROR_INVALIDDATA;



        memcpy(object->rle + object->rle_data_len, buf, buf_size);

        object->rle_data_len += buf_size;

        object->rle_remaining_len -= buf_size;



        return 0;

    }



    if (buf_size <= 7)

        return AVERROR_INVALIDDATA;

    buf_size -= 7;



    

    rle_bitmap_len = bytestream_get_be24(&buf) - 2*2;



    if (buf_size > rle_bitmap_len) {

        av_log(avctx, AV_LOG_ERROR,

               "Buffer dimension %d larger than the expected RLE data %d\n",

               buf_size, rle_bitmap_len);

        return AVERROR_INVALIDDATA;

    }



    

    width  = bytestream_get_be16(&buf);

    height = bytestream_get_be16(&buf);



    

    if (avctx->width < width || avctx->height < height) {

        av_log(avctx, AV_LOG_ERROR, "Bitmap dimensions larger than video.\n");

        return AVERROR_INVALIDDATA;

    }



    object->w = width;

    object->h = height;



    av_fast_padded_malloc(&object->rle, &object->rle_buffer_size, rle_bitmap_len);



    if (!object->rle)

        return AVERROR(ENOMEM);



    memcpy(object->rle, buf, buf_size);

    object->rle_data_len = buf_size;

    object->rle_remaining_len = rle_bitmap_len - buf_size;



    return 0;

}