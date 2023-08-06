static inline void RENAME(hcscale_fast)(SwsContext *c, int16_t *dst,

                                        int dstWidth, const uint8_t *src1,

                                        const uint8_t *src2, int srcW, int xInc)

{

#if ARCH_X86

#if COMPILE_TEMPLATE_MMX2

    int32_t *filterPos = c->hChrFilterPos;

    int16_t *filter    = c->hChrFilter;

    int     canMMX2BeUsed  = c->canMMX2BeUsed;

    void    *mmx2FilterCode= c->chrMmx2FilterCode;

    int i;

#if defined(PIC)

    DECLARE_ALIGNED(8, uint64_t, ebxsave);

#endif

    if (canMMX2BeUsed) {

        __asm__ volatile(

#if defined(PIC)

            "mov          %%"REG_b", %6         \n\t"

#endif

            "pxor             %%mm7, %%mm7      \n\t"

            "mov                 %0, %%"REG_c"  \n\t"

            "mov                 %1, %%"REG_D"  \n\t"

            "mov                 %2, %%"REG_d"  \n\t"

            "mov                 %3, %%"REG_b"  \n\t"

            "xor          %%"REG_a", %%"REG_a"  \n\t" 

            PREFETCH"   (%%"REG_c")             \n\t"

            PREFETCH" 32(%%"REG_c")             \n\t"

            PREFETCH" 64(%%"REG_c")             \n\t"



            CALL_MMX2_FILTER_CODE

            CALL_MMX2_FILTER_CODE

            CALL_MMX2_FILTER_CODE

            CALL_MMX2_FILTER_CODE

            "xor          %%"REG_a", %%"REG_a"  \n\t" 

            "mov                 %5, %%"REG_c"  \n\t" 

            "mov                 %1, %%"REG_D"  \n\t" 

            "add              $"AV_STRINGIFY(VOF)", %%"REG_D"  \n\t"

            PREFETCH"   (%%"REG_c")             \n\t"

            PREFETCH" 32(%%"REG_c")             \n\t"

            PREFETCH" 64(%%"REG_c")             \n\t"



            CALL_MMX2_FILTER_CODE

            CALL_MMX2_FILTER_CODE

            CALL_MMX2_FILTER_CODE

            CALL_MMX2_FILTER_CODE



#if defined(PIC)

            "mov %6, %%"REG_b"    \n\t"

#endif

            :: "m" (src1), "m" (dst), "m" (filter), "m" (filterPos),

            "m" (mmx2FilterCode), "m" (src2)

#if defined(PIC)

            ,"m" (ebxsave)

#endif

            : "%"REG_a, "%"REG_c, "%"REG_d, "%"REG_S, "%"REG_D

#if !defined(PIC)

            ,"%"REG_b

#endif

        );

        for (i=dstWidth-1; (i*xInc)>>16 >=srcW-1; i--) {

            

            dst[i] = src1[srcW-1]*128;

            dst[i+VOFW] = src2[srcW-1]*128;

        }

    } else {

#endif 

        x86_reg dstWidth_reg = dstWidth;

        x86_reg xInc_shr16 = (x86_reg) (xInc >> 16);

        uint16_t xInc_mask = xInc & 0xffff;

        __asm__ volatile(

            "xor %%"REG_a", %%"REG_a"               \n\t" 

            "xor %%"REG_d", %%"REG_d"               \n\t" 

            "xorl    %%ecx, %%ecx                   \n\t" 

            ASMALIGN(4)

            "1:                                     \n\t"

            "mov        %0, %%"REG_S"               \n\t"

            "movzbl  (%%"REG_S", %%"REG_d"), %%edi  \n\t" 

            "movzbl 1(%%"REG_S", %%"REG_d"), %%esi  \n\t" 

            FAST_BILINEAR_X86

            "movw     %%si, (%%"REG_D", %%"REG_a", 2)   \n\t"



            "movzbl    (%5, %%"REG_d"), %%edi       \n\t" 

            "movzbl   1(%5, %%"REG_d"), %%esi       \n\t" 

            FAST_BILINEAR_X86

            "movw     %%si, "AV_STRINGIFY(VOF)"(%%"REG_D", %%"REG_a", 2)   \n\t"



            "addw       %4, %%cx                    \n\t" 

            "adc        %3, %%"REG_d"               \n\t" 

            "add        $1, %%"REG_a"               \n\t"

            "cmp        %2, %%"REG_a"               \n\t"

            " jb        1b                          \n\t"





#if ARCH_X86_64 && AV_GCC_VERSION_AT_LEAST(3,4)

            :: "m" (src1), "m" (dst), "g" (dstWidth_reg), "m" (xInc_shr16), "m" (xInc_mask),

#else

            :: "m" (src1), "m" (dst), "m" (dstWidth_reg), "m" (xInc_shr16), "m" (xInc_mask),

#endif

            "r" (src2)

            : "%"REG_a, "%"REG_d, "%ecx", "%"REG_D, "%esi"

        );

#if COMPILE_TEMPLATE_MMX2

    } 

#endif

#else

    int i;

    unsigned int xpos=0;

    for (i=0;i<dstWidth;i++) {

        register unsigned int xx=xpos>>16;

        register unsigned int xalpha=(xpos&0xFFFF)>>9;

        dst[i]=(src1[xx]*(xalpha^127)+src1[xx+1]*xalpha);

        dst[i+VOFW]=(src2[xx]*(xalpha^127)+src2[xx+1]*xalpha);

        

        xpos+=xInc;

    }

#endif 

}