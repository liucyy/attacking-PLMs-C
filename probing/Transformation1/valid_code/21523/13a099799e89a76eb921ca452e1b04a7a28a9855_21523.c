static void RENAME(yuv2rgb565_1)(SwsContext *c, const uint16_t *buf0,

                                 const uint16_t *ubuf0, const uint16_t *ubuf1,

                                 const uint16_t *vbuf0, const uint16_t *vbuf1,

                                 const uint16_t *abuf0, uint8_t *dest,

                                 int dstW, int uvalpha, enum PixelFormat dstFormat,

                                 int flags, int y)

{

    const uint16_t *buf1= buf0; 



    if (uvalpha < 2048) { 

        __asm__ volatile(

            "mov %%"REG_b", "ESP_OFFSET"(%5)        \n\t"

            "mov        %4, %%"REG_b"               \n\t"

            "push %%"REG_BP"                        \n\t"

            YSCALEYUV2RGB1(%%REGBP, %5)

            "pxor    %%mm7, %%mm7                   \n\t"

            

#ifdef DITHER1XBPP

            "paddusb "BLUE_DITHER"(%5), %%mm2      \n\t"

            "paddusb "GREEN_DITHER"(%5), %%mm4      \n\t"

            "paddusb "RED_DITHER"(%5), %%mm5      \n\t"

#endif

            WRITERGB16(%%REGb, 8280(%5), %%REGBP)

            "pop %%"REG_BP"                         \n\t"

            "mov "ESP_OFFSET"(%5), %%"REG_b"        \n\t"

            :: "c" (buf0), "d" (buf1), "S" (ubuf0), "D" (ubuf1), "m" (dest),

               "a" (&c->redDither)

        );

    } else {

        __asm__ volatile(

            "mov %%"REG_b", "ESP_OFFSET"(%5)        \n\t"

            "mov        %4, %%"REG_b"               \n\t"

            "push %%"REG_BP"                        \n\t"

            YSCALEYUV2RGB1b(%%REGBP, %5)

            "pxor    %%mm7, %%mm7                   \n\t"

            

#ifdef DITHER1XBPP

            "paddusb "BLUE_DITHER"(%5), %%mm2      \n\t"

            "paddusb "GREEN_DITHER"(%5), %%mm4      \n\t"

            "paddusb "RED_DITHER"(%5), %%mm5      \n\t"

#endif

            WRITERGB16(%%REGb, 8280(%5), %%REGBP)

            "pop %%"REG_BP"                         \n\t"

            "mov "ESP_OFFSET"(%5), %%"REG_b"        \n\t"

            :: "c" (buf0), "d" (buf1), "S" (ubuf0), "D" (ubuf1), "m" (dest),

               "a" (&c->redDither)

        );

    }

}