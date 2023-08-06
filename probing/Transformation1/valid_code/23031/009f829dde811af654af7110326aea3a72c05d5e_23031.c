static inline void RENAME(yuv2yuyv422_1)(SwsContext *c, const uint16_t *buf0,

                                         const uint16_t *ubuf0, const uint16_t *ubuf1,

                                         const uint16_t *vbuf0, const uint16_t *vbuf1,

                                         const uint16_t *abuf0, uint8_t *dest,

                                         int dstW, int uvalpha, enum PixelFormat dstFormat,

                                         int flags, int y)

{

    x86_reg uv_off = c->uv_off << 1;

    const uint16_t *buf1= buf0; 



    if (uvalpha < 2048) { 

        __asm__ volatile(

            "mov %%"REG_b", "ESP_OFFSET"(%5)        \n\t"

            "mov        %4, %%"REG_b"               \n\t"

            "push %%"REG_BP"                        \n\t"

            YSCALEYUV2PACKED1(%%REGBP, %5, %6)

            WRITEYUY2(%%REGb, 8280(%5), %%REGBP)

            "pop %%"REG_BP"                         \n\t"

            "mov "ESP_OFFSET"(%5), %%"REG_b"        \n\t"

            :: "c" (buf0), "d" (buf1), "S" (ubuf0), "D" (ubuf1), "m" (dest),

               "a" (&c->redDither), "m"(uv_off)

        );

    } else {

        __asm__ volatile(

            "mov %%"REG_b", "ESP_OFFSET"(%5)        \n\t"

            "mov        %4, %%"REG_b"               \n\t"

            "push %%"REG_BP"                        \n\t"

            YSCALEYUV2PACKED1b(%%REGBP, %5, %6)

            WRITEYUY2(%%REGb, 8280(%5), %%REGBP)

            "pop %%"REG_BP"                         \n\t"

            "mov "ESP_OFFSET"(%5), %%"REG_b"        \n\t"

            :: "c" (buf0), "d" (buf1), "S" (ubuf0), "D" (ubuf1), "m" (dest),

               "a" (&c->redDither), "m"(uv_off)

        );

    }

}