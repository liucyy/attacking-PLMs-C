void ff_imdct_calc_sse(MDCTContext *s, FFTSample *output,

                       const FFTSample *input, FFTSample *tmp)

{

    x86_reg k;

    long n8, n4, n2, n;

    const uint16_t *revtab = s->fft.revtab;

    const FFTSample *tcos = s->tcos;

    const FFTSample *tsin = s->tsin;

    const FFTSample *in1, *in2;

    FFTComplex *z = (FFTComplex *)tmp;



    n = 1 << s->nbits;

    n2 = n >> 1;

    n4 = n >> 2;

    n8 = n >> 3;



#ifdef ARCH_X86_64

    asm volatile ("movaps %0, %%xmm8\n\t"::"m"(*p1m1p1m1));

#define P1M1P1M1 "%%xmm8"

#else

#define P1M1P1M1 "%4"

#endif



    

    in1 = input;

    in2 = input + n2 - 4;



    

    for (k = 0; k < n4; k += 4) {

        asm volatile (

            "movaps          %0, %%xmm0 \n\t"   

            "movaps          %1, %%xmm3 \n\t"   

            "movaps    -16+1*%0, %%xmm4 \n\t"   

            "movaps     16+1*%1, %%xmm7 \n\t"   

            "movlps          %2, %%xmm1 \n\t"   

            "movlps          %3, %%xmm2 \n\t"   

            "movlps      8+1*%2, %%xmm5 \n\t"   

            "movlps      8+1*%3, %%xmm6 \n\t"   

            "shufps $95, %%xmm0, %%xmm0 \n\t"   

            "shufps $160,%%xmm3, %%xmm3 \n\t"   

            "shufps $95, %%xmm4, %%xmm4 \n\t"   

            "shufps $160,%%xmm7, %%xmm7 \n\t"   

            "unpcklps    %%xmm2, %%xmm1 \n\t"   

            "unpcklps    %%xmm6, %%xmm5 \n\t"   

            "movaps      %%xmm1, %%xmm2 \n\t"   

            "movaps      %%xmm5, %%xmm6 \n\t"   

            "xorps   "P1M1P1M1", %%xmm2 \n\t"   

            "xorps   "P1M1P1M1", %%xmm6 \n\t"   

            "mulps       %%xmm1, %%xmm0 \n\t"   

            "mulps       %%xmm5, %%xmm4 \n\t"   

            "shufps $177,%%xmm2, %%xmm2 \n\t"   

            "shufps $177,%%xmm6, %%xmm6 \n\t"   

            "mulps       %%xmm2, %%xmm3 \n\t"   

            "mulps       %%xmm6, %%xmm7 \n\t"   

            "addps       %%xmm3, %%xmm0 \n\t"   

            "addps       %%xmm7, %%xmm4 \n\t"   

            ::"m"(in2[-2*k]), "m"(in1[2*k]),

              "m"(tcos[k]), "m"(tsin[k])

#ifndef ARCH_X86_64

              ,"m"(*p1m1p1m1)

#endif

        );

        

        asm (

            "movlps      %%xmm0, %0     \n\t"

            "movhps      %%xmm0, %1     \n\t"

            "movlps      %%xmm4, %2     \n\t"

            "movhps      %%xmm4, %3     \n\t"

            :"=m"(z[revtab[k]]), "=m"(z[revtab[k + 1]]),

             "=m"(z[revtab[k + 2]]), "=m"(z[revtab[k + 3]])

        );

    }



    ff_fft_calc_sse(&s->fft, z);



#ifndef ARCH_X86_64

#undef P1M1P1M1

#define P1M1P1M1 "%3"

#endif



    

    for (k = 0; k < n4; k += 4) {

        asm (

            "movaps          %0, %%xmm0 \n\t"   

            "movaps     16+1*%0, %%xmm4 \n\t"   

            "movlps          %1, %%xmm1 \n\t"   

            "movlps      8+1*%1, %%xmm5 \n\t"   

            "movaps      %%xmm0, %%xmm3 \n\t"   

            "movaps      %%xmm4, %%xmm7 \n\t"   

            "movlps          %2, %%xmm2 \n\t"   

            "movlps      8+1*%2, %%xmm6 \n\t"   

            "shufps $160,%%xmm0, %%xmm0 \n\t"   

            "shufps $245,%%xmm3, %%xmm3 \n\t"   

            "shufps $160,%%xmm4, %%xmm4 \n\t"   

            "shufps $245,%%xmm7, %%xmm7 \n\t"   

            "unpcklps    %%xmm2, %%xmm1 \n\t"   

            "unpcklps    %%xmm6, %%xmm5 \n\t"   

            "movaps      %%xmm1, %%xmm2 \n\t"   

            "movaps      %%xmm5, %%xmm6 \n\t"   

            "xorps   "P1M1P1M1", %%xmm2 \n\t"   

            "mulps       %%xmm1, %%xmm0 \n\t"   

            "xorps   "P1M1P1M1", %%xmm6 \n\t"   

            "mulps       %%xmm5, %%xmm4 \n\t"   

            "shufps $177,%%xmm2, %%xmm2 \n\t"   

            "shufps $177,%%xmm6, %%xmm6 \n\t"   

            "mulps       %%xmm2, %%xmm3 \n\t"   

            "mulps       %%xmm6, %%xmm7 \n\t"   

            "addps       %%xmm3, %%xmm0 \n\t"   

            "addps       %%xmm7, %%xmm4 \n\t"   

            "movaps      %%xmm0, %0     \n\t"

            "movaps      %%xmm4, 16+1*%0\n\t"

            :"+m"(z[k])

            :"m"(tcos[k]), "m"(tsin[k])

#ifndef ARCH_X86_64

             ,"m"(*p1m1p1m1)

#endif

        );

    }



    

    k = 16-n;

    asm volatile("movaps %0, %%xmm7 \n\t"::"m"(*m1m1m1m1));

    asm volatile(

        "1: \n\t"

        "movaps  -16(%4,%0), %%xmm1 \n\t"   

        "neg %0 \n\t"

        "movaps     (%4,%0), %%xmm0 \n\t"   

        "xorps       %%xmm7, %%xmm0 \n\t"   

        "movaps      %%xmm0, %%xmm2 \n\t"   

        "shufps $141,%%xmm1, %%xmm0 \n\t"   

        "shufps $216,%%xmm1, %%xmm2 \n\t"   

        "shufps $156,%%xmm0, %%xmm0 \n\t"   

        "shufps $156,%%xmm2, %%xmm2 \n\t"   

        "movaps      %%xmm0, (%1,%0) \n\t"  

        "movaps      %%xmm2, (%2,%0) \n\t"  

        "neg %0 \n\t"

        "shufps $27, %%xmm0, %%xmm0 \n\t"   

        "xorps       %%xmm7, %%xmm0 \n\t"   

        "shufps $27, %%xmm2, %%xmm2 \n\t"   

        "movaps      %%xmm0, -16(%2,%0) \n\t" 

        "movaps      %%xmm2, -16(%3,%0) \n\t" 

        "add $16, %0 \n\t"

        "jle 1b \n\t"

        :"+r"(k)

        :"r"(output), "r"(output+n2), "r"(output+n), "r"(z+n8)

        :"memory"

    );

}