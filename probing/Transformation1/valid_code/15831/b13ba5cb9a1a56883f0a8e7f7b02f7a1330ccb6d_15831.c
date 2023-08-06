void ff_get_unscaled_swscale(SwsContext *c)

{

    const enum PixelFormat srcFormat = c->srcFormat;

    const enum PixelFormat dstFormat = c->dstFormat;

    const int flags = c->flags;

    const int dstH = c->dstH;

    int needsDither;



    needsDither= isAnyRGB(dstFormat)

        &&  c->dstFormatBpp < 24

        && (c->dstFormatBpp < c->srcFormatBpp || (!isAnyRGB(srcFormat)));



    

    if ((srcFormat == PIX_FMT_YUV420P || srcFormat == PIX_FMT_YUVA420P) && (dstFormat == PIX_FMT_NV12 || dstFormat == PIX_FMT_NV21)) {

        c->swScale= planarToNv12Wrapper;

    }

    

    if ((srcFormat==PIX_FMT_YUV420P || srcFormat==PIX_FMT_YUV422P || srcFormat==PIX_FMT_YUVA420P) && isAnyRGB(dstFormat)

        && !(flags & SWS_ACCURATE_RND) && !(dstH&1)) {

        c->swScale= ff_yuv2rgb_get_func_ptr(c);

    }



    if (srcFormat==PIX_FMT_YUV410P && (dstFormat==PIX_FMT_YUV420P || dstFormat==PIX_FMT_YUVA420P) && !(flags & SWS_BITEXACT)) {

        c->swScale= yvu9ToYv12Wrapper;

    }



    

    if (srcFormat==PIX_FMT_BGR24 && (dstFormat==PIX_FMT_YUV420P || dstFormat==PIX_FMT_YUVA420P) && !(flags & SWS_ACCURATE_RND))

        c->swScale= bgr24ToYv12Wrapper;



    

    if (   isAnyRGB(srcFormat)

        && isAnyRGB(dstFormat)

        && srcFormat != PIX_FMT_BGR8      && dstFormat != PIX_FMT_BGR8

        && srcFormat != PIX_FMT_RGB8      && dstFormat != PIX_FMT_RGB8

        && srcFormat != PIX_FMT_BGR4      && dstFormat != PIX_FMT_BGR4

        && srcFormat != PIX_FMT_RGB4      && dstFormat != PIX_FMT_RGB4

        && srcFormat != PIX_FMT_BGR4_BYTE && dstFormat != PIX_FMT_BGR4_BYTE

        && srcFormat != PIX_FMT_RGB4_BYTE && dstFormat != PIX_FMT_RGB4_BYTE

        && srcFormat != PIX_FMT_MONOBLACK && dstFormat != PIX_FMT_MONOBLACK

        && srcFormat != PIX_FMT_MONOWHITE && dstFormat != PIX_FMT_MONOWHITE

        && srcFormat != PIX_FMT_RGB48LE   && dstFormat != PIX_FMT_RGB48LE

        && srcFormat != PIX_FMT_RGB48BE   && dstFormat != PIX_FMT_RGB48BE

        && srcFormat != PIX_FMT_BGR48LE   && dstFormat != PIX_FMT_BGR48LE

        && srcFormat != PIX_FMT_BGR48BE   && dstFormat != PIX_FMT_BGR48BE

        && (!needsDither || (c->flags&(SWS_FAST_BILINEAR|SWS_POINT))))

        c->swScale= rgbToRgbWrapper;



#define isByteRGB(f) (\

        f == PIX_FMT_RGB32   ||\

        f == PIX_FMT_RGB32_1 ||\

        f == PIX_FMT_RGB24   ||\

        f == PIX_FMT_BGR32   ||\

        f == PIX_FMT_BGR32_1 ||\

        f == PIX_FMT_BGR24)



    if (isAnyRGB(srcFormat) && isPlanar(srcFormat) && isByteRGB(dstFormat))

        c->swScale= planarRgbToRgbWrapper;



    if ((usePal(srcFormat) && (

        dstFormat == PIX_FMT_RGB32   ||

        dstFormat == PIX_FMT_RGB32_1 ||

        dstFormat == PIX_FMT_RGB24   ||

        dstFormat == PIX_FMT_BGR32   ||

        dstFormat == PIX_FMT_BGR32_1 ||

        dstFormat == PIX_FMT_BGR24)))

        c->swScale= palToRgbWrapper;



    if (srcFormat == PIX_FMT_YUV422P) {

        if (dstFormat == PIX_FMT_YUYV422)

            c->swScale= yuv422pToYuy2Wrapper;

        else if (dstFormat == PIX_FMT_UYVY422)

            c->swScale= yuv422pToUyvyWrapper;

    }



    

    if (c->flags&(SWS_FAST_BILINEAR|SWS_POINT)) {

        

        if (srcFormat == PIX_FMT_YUV420P || srcFormat == PIX_FMT_YUVA420P) {

            if (dstFormat == PIX_FMT_YUYV422)

                c->swScale= planarToYuy2Wrapper;

            else if (dstFormat == PIX_FMT_UYVY422)

                c->swScale= planarToUyvyWrapper;

        }

    }

    if(srcFormat == PIX_FMT_YUYV422 && (dstFormat == PIX_FMT_YUV420P || dstFormat == PIX_FMT_YUVA420P))

        c->swScale= yuyvToYuv420Wrapper;

    if(srcFormat == PIX_FMT_UYVY422 && (dstFormat == PIX_FMT_YUV420P || dstFormat == PIX_FMT_YUVA420P))

        c->swScale= uyvyToYuv420Wrapper;

    if(srcFormat == PIX_FMT_YUYV422 && dstFormat == PIX_FMT_YUV422P)

        c->swScale= yuyvToYuv422Wrapper;

    if(srcFormat == PIX_FMT_UYVY422 && dstFormat == PIX_FMT_YUV422P)

        c->swScale= uyvyToYuv422Wrapper;



    

    if (  srcFormat == dstFormat

        || (srcFormat == PIX_FMT_YUVA420P && dstFormat == PIX_FMT_YUV420P)

        || (srcFormat == PIX_FMT_YUV420P && dstFormat == PIX_FMT_YUVA420P)

        || (isPlanarYUV(srcFormat) && isGray(dstFormat))

        || (isPlanarYUV(dstFormat) && isGray(srcFormat))

        || (isGray(dstFormat) && isGray(srcFormat))

        || (isPlanarYUV(srcFormat) && isPlanarYUV(dstFormat)

            && c->chrDstHSubSample == c->chrSrcHSubSample

            && c->chrDstVSubSample == c->chrSrcVSubSample

            && dstFormat != PIX_FMT_NV12 && dstFormat != PIX_FMT_NV21

            && srcFormat != PIX_FMT_NV12 && srcFormat != PIX_FMT_NV21))

    {

        if (isPacked(c->srcFormat))

            c->swScale= packedCopyWrapper;

        else 

            c->swScale= planarCopyWrapper;

    }



    if (ARCH_BFIN)

        ff_bfin_get_unscaled_swscale(c);

    if (HAVE_ALTIVEC)

        ff_swscale_get_unscaled_altivec(c);

}