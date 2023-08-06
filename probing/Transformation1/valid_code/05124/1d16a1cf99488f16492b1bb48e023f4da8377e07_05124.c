static void ff_h264_idct_add_mmx(uint8_t *dst, int16_t *block, int stride)

{

    

    __asm__ volatile(

        "movq   (%0), %%mm0 \n\t"

        "movq  8(%0), %%mm1 \n\t"

        "movq 16(%0), %%mm2 \n\t"

        "movq 24(%0), %%mm3 \n\t"

    :: "r"(block) );



    __asm__ volatile(

        

        IDCT4_1D( %%mm2, %%mm1, %%mm0, %%mm3, %%mm4 )



        "movq      %0,    %%mm6 \n\t"

        

        TRANSPOSE4( %%mm3, %%mm1, %%mm0, %%mm2, %%mm4 )



        "paddw     %%mm6, %%mm3 \n\t"



        

        IDCT4_1D( %%mm4, %%mm2, %%mm3, %%mm0, %%mm1 )



        "pxor %%mm7, %%mm7    \n\t"

    :: "m"(ff_pw_32));



    __asm__ volatile(

    STORE_DIFF_4P( %%mm0, %%mm1, %%mm7)

        "add %1, %0             \n\t"

    STORE_DIFF_4P( %%mm2, %%mm1, %%mm7)

        "add %1, %0             \n\t"

    STORE_DIFF_4P( %%mm3, %%mm1, %%mm7)

        "add %1, %0             \n\t"

    STORE_DIFF_4P( %%mm4, %%mm1, %%mm7)

        : "+r"(dst)

        : "r" ((x86_reg)stride)

    );

}