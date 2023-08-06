static void ff_h264_idct_dc_add8_mmx2(uint8_t *dst, int16_t *block, int stride)

{

    __asm__ volatile(

        "movd             %0, %%mm0 \n\t"   

        "punpcklwd        %1, %%mm0 \n\t"   

        "paddsw           %2, %%mm0 \n\t"

        "psraw            $6, %%mm0 \n\t"

        "punpcklwd     %%mm0, %%mm0 \n\t"   

        "pxor          %%mm1, %%mm1 \n\t"   

        "psubw         %%mm0, %%mm1 \n\t"   

        "packuswb      %%mm1, %%mm0 \n\t"   

        "pshufw $0xFA, %%mm0, %%mm1 \n\t"   

        "punpcklwd     %%mm0, %%mm0 \n\t"   

        ::"m"(block[ 0]),

          "m"(block[16]),

          "m"(ff_pw_32)

    );

    __asm__ volatile(

        "movq          %0, %%mm2 \n\t"

        "movq          %1, %%mm3 \n\t"

        "movq          %2, %%mm4 \n\t"

        "movq          %3, %%mm5 \n\t"

        "paddusb    %%mm0, %%mm2 \n\t"

        "paddusb    %%mm0, %%mm3 \n\t"

        "paddusb    %%mm0, %%mm4 \n\t"

        "paddusb    %%mm0, %%mm5 \n\t"

        "psubusb    %%mm1, %%mm2 \n\t"

        "psubusb    %%mm1, %%mm3 \n\t"

        "psubusb    %%mm1, %%mm4 \n\t"

        "psubusb    %%mm1, %%mm5 \n\t"

        "movq       %%mm2, %0    \n\t"

        "movq       %%mm3, %1    \n\t"

        "movq       %%mm4, %2    \n\t"

        "movq       %%mm5, %3    \n\t"

        :"+m"(*(uint64_t*)(dst+0*stride)),

         "+m"(*(uint64_t*)(dst+1*stride)),

         "+m"(*(uint64_t*)(dst+2*stride)),

         "+m"(*(uint64_t*)(dst+3*stride))

    );

}