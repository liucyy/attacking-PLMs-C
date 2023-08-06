static int xpm_decode_frame(AVCodecContext *avctx, void *data,

                            int *got_frame, AVPacket *avpkt)

{

    XPMDecContext *x = avctx->priv_data;

    AVFrame *p=data;

    const uint8_t *end, *ptr = avpkt->data;

    int ncolors, cpp, ret, i, j;

    int64_t size;

    uint32_t *dst;



    avctx->pix_fmt = AV_PIX_FMT_BGRA;



    end = avpkt->data + avpkt->size;

    while (memcmp(ptr, "\n", 10) && ptr < end - 10)

        ptr++;



    if (ptr >= end) {

        av_log(avctx, AV_LOG_ERROR, "missing signature\n");

        return AVERROR_INVALIDDATA;

    }



    ptr += mod_strcspn(ptr, "\"");

    if (sscanf(ptr, "\"%u %u %u %u\",",

               &avctx->width, &avctx->height, &ncolors, &cpp) != 4) {

        av_log(avctx, AV_LOG_ERROR, "missing image parameters\n");

        return AVERROR_INVALIDDATA;

    }



    if ((ret = ff_set_dimensions(avctx, avctx->width, avctx->height)) < 0)

        return ret;



    if ((ret = ff_get_buffer(avctx, p, 0)) < 0)

        return ret;



    if (cpp <= 0 || cpp >= 5) {

        av_log(avctx, AV_LOG_ERROR, "unsupported/invalid number of chars per pixel: %d\n", cpp);

        return AVERROR_INVALIDDATA;

    }



    size = 1;

    for (i = 0; i < cpp; i++)

        size *= 94;



    if (ncolors <= 0 || ncolors > size) {

        av_log(avctx, AV_LOG_ERROR, "invalid number of colors: %d\n", ncolors);

        return AVERROR_INVALIDDATA;

    }



    size *= 4;



    av_fast_padded_malloc(&x->pixels, &x->pixels_size, size);

    if (!x->pixels)

        return AVERROR(ENOMEM);



    ptr += mod_strcspn(ptr, ",") + 1;

    for (i = 0; i < ncolors; i++) {

        const uint8_t *index;

        int len;



        ptr += mod_strcspn(ptr, "\"") + 1;

        if (ptr + cpp > end)

            return AVERROR_INVALIDDATA;

        index = ptr;

        ptr += cpp;



        ptr = strstr(ptr, "c ");

        if (ptr) {

            ptr += 2;

        } else {

            return AVERROR_INVALIDDATA;

        }



        len = strcspn(ptr, "\" ");



        if ((ret = ascii2index(index, cpp)) < 0)

            return ret;



        x->pixels[ret] = color_string_to_rgba(ptr, len);

        ptr += mod_strcspn(ptr, ",") + 1;

    }



    for (i = 0; i < avctx->height; i++) {

        dst = (uint32_t *)(p->data[0] + i * p->linesize[0]);

        ptr += mod_strcspn(ptr, "\"") + 1;



        for (j = 0; j < avctx->width; j++) {

            if (ptr + cpp > end)

                return AVERROR_INVALIDDATA;



            if ((ret = ascii2index(ptr, cpp)) < 0)

                return ret;



            *dst++ = x->pixels[ret];

            ptr += cpp;

        }

        ptr += mod_strcspn(ptr, ",") + 1;

    }



    p->key_frame = 1;

    p->pict_type = AV_PICTURE_TYPE_I;



    *got_frame = 1;



    return avpkt->size;

}