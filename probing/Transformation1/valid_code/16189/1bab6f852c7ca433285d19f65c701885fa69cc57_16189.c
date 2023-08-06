static void RENAME(yuv2yuyv422_1)(SwsContext *c, const int16_t *buf0,

                                  const int16_t *ubuf[2], const int16_t *bguf[2],

                                  const int16_t *abuf0, uint8_t *dest,

                                  int dstW, int uvalpha, int y)

{

    const int16_t *ubuf0 = ubuf[0], *ubuf1 = ubuf[1];

    const int16_t *buf1= buf0; 



    if (uvalpha < 2048) { 

        __asm__ volatile(

            "mov %%"REG_b", "ESP_OFFSET"(%5)        \n\t"

            "mov        %4, %%"REG_b"               \n\t"

            "push %%"REG_BP"                        \n\t"

            YSCALEYUV2PACKED1(%%REGBP, %5)

            WRITEYUY2(%%REGb, 8280(%5), %%REGBP)

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

            YSCALEYUV2PACKED1b(%%REGBP, %5)

            WRITEYUY2(%%REGb, 8280(%5), %%REGBP)

            "pop %%"REG_BP"                         \n\t"

            "mov "ESP_OFFSET"(%5), %%"REG_b"        \n\t"

            :: "c" (buf0), "d" (buf1), "S" (ubuf0), "D" (ubuf1), "m" (dest),

               "a" (&c->redDither)

        );

    }

}