void ff_imdct_calc_sse(MDCTContext *s, FFTSample *output,

                       const FFTSample *input, FFTSample *tmp)

{

    long k, n8, n4, n2, n;

    const uint16_t *revtab = s->fft.revtab;

    const FFTSample *tcos = s->tcos;

    const FFTSample *tsin = s->tsin;

    const FFTSample *in1, *in2;

    FFTComplex *z = (FFTComplex *)tmp;



    n = 1 << s->nbits;

    n2 = n >> 1;

    n4 = n >> 2;

    n8 = n >> 3;



    asm volatile ("movaps %0, %%xmm7\n\t"::"m"(*p1m1p1m1));



    

    in1 = input;

    in2 = input + n2 - 4;



    

    for (k = 0; k < n4; k += 2) {

        asm volatile (

            "movaps          %0, %%xmm0 \n\t"   

            "movaps          %1, %%xmm3 \n\t"   

            "movlps          %2, %%xmm1 \n\t"   

            "movlps          %3, %%xmm2 \n\t"   

            "shufps $95, %%xmm0, %%xmm0 \n\t"   

            "shufps $160,%%xmm3, %%xmm3 \n\t"   

            "unpcklps    %%xmm2, %%xmm1 \n\t"   

            "movaps      %%xmm1, %%xmm2 \n\t"   

            "xorps       %%xmm7, %%xmm2 \n\t"   

            "mulps       %%xmm1, %%xmm0 \n\t"   

            "shufps $177,%%xmm2, %%xmm2 \n\t"   

            "mulps       %%xmm2, %%xmm3 \n\t"   

            "addps       %%xmm3, %%xmm0 \n\t"   

            ::"m"(in2[-2*k]), "m"(in1[2*k]),

              "m"(tcos[k]), "m"(tsin[k])

        );

        

        asm (

            "movlps      %%xmm0, %0     \n\t"

            "movhps      %%xmm0, %1     \n\t"

            :"=m"(z[revtab[k]]), "=m"(z[revtab[k + 1]])

        );

    }



    ff_fft_calc_sse(&s->fft, z);



    

    asm volatile ("movaps %0, %%xmm7\n\t"::"m"(*p1m1p1m1));



    

    for (k = 0; k < n4; k += 2) {

        asm (

            "movaps          %0, %%xmm0 \n\t"   

            "movlps          %1, %%xmm1 \n\t"   

            "movaps      %%xmm0, %%xmm3 \n\t"   

            "movlps          %2, %%xmm2 \n\t"   

            "shufps $160,%%xmm0, %%xmm0 \n\t"   

            "shufps $245,%%xmm3, %%xmm3 \n\t"   

            "unpcklps    %%xmm2, %%xmm1 \n\t"   

            "movaps      %%xmm1, %%xmm2 \n\t"   

            "xorps       %%xmm7, %%xmm2 \n\t"   

            "mulps       %%xmm1, %%xmm0 \n\t"   

            "shufps $177,%%xmm2, %%xmm2 \n\t"   

            "mulps       %%xmm2, %%xmm3 \n\t"   

            "addps       %%xmm3, %%xmm0 \n\t"   

            "movaps      %%xmm0, %0     \n\t"

            :"+m"(z[k])

            :"m"(tcos[k]), "m"(tsin[k])

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