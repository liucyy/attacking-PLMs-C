static inline void h264_loop_filter_luma_mmx2(uint8_t *pix, int stride, int alpha1, int beta1, int8_t *tc0)

{

    DECLARE_ALIGNED_8(uint64_t, tmp0[2]);



    __asm__ volatile(

        "movq    (%1,%3), %%mm0    \n\t" 

        "movq    (%1,%3,2), %%mm1  \n\t" 

        "movq    (%2),    %%mm2    \n\t" 

        "movq    (%2,%3), %%mm3    \n\t" 

        H264_DEBLOCK_MASK(%6, %7)



        "movd      %5,    %%mm4    \n\t"

        "punpcklbw %%mm4, %%mm4    \n\t"

        "punpcklwd %%mm4, %%mm4    \n\t"

        "pcmpeqb   %%mm3, %%mm3    \n\t"

        "movq      %%mm4, %%mm6    \n\t"

        "pcmpgtb   %%mm3, %%mm4    \n\t"

        "movq      %%mm6, 8+%0     \n\t"

        "pand      %%mm4, %%mm7    \n\t"

        "movq      %%mm7, %0       \n\t"



        

        "movq     (%1),   %%mm3    \n\t" 

        DIFF_GT2_MMX(%%mm1, %%mm3, %%mm5, %%mm6, %%mm4) 

        "pand     %%mm7,  %%mm6    \n\t" 

        "pand     8+%0,   %%mm7    \n\t" 

        "movq     %%mm7,  %%mm4    \n\t"

        "psubb    %%mm6,  %%mm7    \n\t"

        "pand     %%mm4,  %%mm6    \n\t" 

        H264_DEBLOCK_Q1(%%mm0, %%mm3, "(%1)", "(%1,%3)", %%mm6, %%mm4)



        

        "movq    (%2,%3,2), %%mm4  \n\t" 

        DIFF_GT2_MMX(%%mm2, %%mm4, %%mm5, %%mm6, %%mm3) 

        "pand     %0,     %%mm6    \n\t"

        "movq     8+%0,   %%mm5    \n\t" 

        "pand     %%mm6,  %%mm5    \n\t"

        "psubb    %%mm6,  %%mm7    \n\t"

        "movq    (%2,%3), %%mm3    \n\t"

        H264_DEBLOCK_Q1(%%mm3, %%mm4, "(%2,%3,2)", "(%2,%3)", %%mm5, %%mm6)



        

        H264_DEBLOCK_P0_Q0(%8, unused)

        "movq      %%mm1, (%1,%3,2) \n\t"

        "movq      %%mm2, (%2)      \n\t"



        : "=m"(*tmp0)

        : "r"(pix-3*stride), "r"(pix), "r"((x86_reg)stride),

          "m"(*tmp0), "m"(*(uint32_t*)tc0), "m"(alpha1), "m"(beta1),

          "m"(ff_bone)

    );

}