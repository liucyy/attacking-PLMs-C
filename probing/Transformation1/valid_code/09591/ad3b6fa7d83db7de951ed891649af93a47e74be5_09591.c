static int bayer_to_rgb24_wrapper(SwsContext *c, const uint8_t* src[], int srcStride[], int srcSliceY,

                                  int srcSliceH, uint8_t* dst[], int dstStride[])

{

    uint8_t *dstPtr= dst[0];

    const uint8_t *srcPtr= src[0];

    int i;

    void (*copy)       (const uint8_t *src, int src_stride, uint8_t *dst, int dst_stride, int width);

    void (*interpolate)(const uint8_t *src, int src_stride, uint8_t *dst, int dst_stride, int width);



    switch(c->srcFormat) {

#define CASE(pixfmt, prefix) \

    case pixfmt: copy        = bayer_##prefix##_to_rgb24_copy; \

                 interpolate = bayer_##prefix##_to_rgb24_interpolate; \

                 break;

    CASE(AV_PIX_FMT_BAYER_BGGR8,    bggr8)

    CASE(AV_PIX_FMT_BAYER_BGGR16LE, bggr16le)

    CASE(AV_PIX_FMT_BAYER_BGGR16BE, bggr16be)

    CASE(AV_PIX_FMT_BAYER_RGGB8,    rggb8)

    CASE(AV_PIX_FMT_BAYER_RGGB16LE, rggb16le)

    CASE(AV_PIX_FMT_BAYER_RGGB16BE, rggb16be)

    CASE(AV_PIX_FMT_BAYER_GBRG8,    gbrg8)

    CASE(AV_PIX_FMT_BAYER_GBRG16LE, gbrg16le)

    CASE(AV_PIX_FMT_BAYER_GBRG16BE, gbrg16be)

    CASE(AV_PIX_FMT_BAYER_GRBG8,    grbg8)

    CASE(AV_PIX_FMT_BAYER_GRBG16LE, grbg16le)

    CASE(AV_PIX_FMT_BAYER_GRBG16BE, grbg16be)

#undef CASE

    default: return 0;

    }



    copy(srcPtr, srcStride[0], dstPtr, dstStride[0], c->srcW);

    srcPtr += 2 * srcStride[0];

    dstPtr += 2 * dstStride[0];



    for (i = 2; i < srcSliceH - 2; i += 2) {

        interpolate(srcPtr, srcStride[0], dstPtr, dstStride[0], c->srcW);

        srcPtr += 2 * srcStride[0];

        dstPtr += 2 * dstStride[0];

    }



    copy(srcPtr, srcStride[0], dstPtr, dstStride[0], c->srcW);

    return srcSliceH;

}