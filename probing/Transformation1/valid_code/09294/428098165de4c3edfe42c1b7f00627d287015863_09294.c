static inline int RENAME(yuv420_rgb24)(SwsContext *c, uint8_t* src[], int srcStride[], int srcSliceY,

             int srcSliceH, uint8_t* dst[], int dstStride[]){

    int y, h_size;



    if(c->srcFormat == PIX_FMT_YUV422P){

	srcStride[1] *= 2;

	srcStride[2] *= 2;

    }



    h_size= (c->dstW+7)&~7;

    if(h_size*3 > FFABS(dstStride[0])) h_size-=8;



    __asm__ __volatile__ ("pxor %mm4, %mm4;"  );



    for (y= 0; y<srcSliceH; y++ ) {

	uint8_t *_image = dst[0] + (y+srcSliceY)*dstStride[0];

	uint8_t *_py = src[0] + y*srcStride[0];

	uint8_t *_pu = src[1] + (y>>1)*srcStride[1];

	uint8_t *_pv = src[2] + (y>>1)*srcStride[2];

	long index= -h_size/2;



	    

	    __asm__ __volatile__ (

	

		     "movd (%2, %0), %%mm0;" 

		     "movd (%3, %0), %%mm1;" 

		     "movq (%5, %0, 2), %%mm6;" 



		    "1:				\n\t"

YUV2RGB

	

#ifdef HAVE_MMX2

			"movq "MANGLE(M24A)", %%mm4	\n\t"

			"movq "MANGLE(M24C)", %%mm7	\n\t"

			"pshufw $0x50, %%mm0, %%mm5	\n\t" 

			"pshufw $0x50, %%mm2, %%mm3	\n\t" 

			"pshufw $0x00, %%mm1, %%mm6	\n\t" 



			"pand %%mm4, %%mm5		\n\t" 

			"pand %%mm4, %%mm3		\n\t" 

			"pand %%mm7, %%mm6		\n\t" 



			"psllq $8, %%mm3		\n\t" 

			"por %%mm5, %%mm6		\n\t"

			"por %%mm3, %%mm6		\n\t"

			MOVNTQ" %%mm6, (%1)		\n\t"



			"psrlq $8, %%mm2		\n\t" 

			"pshufw $0xA5, %%mm0, %%mm5	\n\t" 

			"pshufw $0x55, %%mm2, %%mm3	\n\t" 

			"pshufw $0xA5, %%mm1, %%mm6	\n\t" 



			"pand "MANGLE(M24B)", %%mm5	\n\t" 

			"pand %%mm7, %%mm3		\n\t" 

			"pand %%mm4, %%mm6		\n\t" 



			"por %%mm5, %%mm3		\n\t" 

			"por %%mm3, %%mm6		\n\t"

			MOVNTQ" %%mm6, 8(%1)		\n\t"



			"pshufw $0xFF, %%mm0, %%mm5	\n\t" 

			"pshufw $0xFA, %%mm2, %%mm3	\n\t" 

			"pshufw $0xFA, %%mm1, %%mm6	\n\t" 

			"movd 4 (%2, %0), %%mm0;" 



			"pand %%mm7, %%mm5		\n\t" 

			"pand %%mm4, %%mm3		\n\t" 

			"pand "MANGLE(M24B)", %%mm6	\n\t" 

			"movd 4 (%3, %0), %%mm1;" 

\

			"por %%mm5, %%mm3		\n\t"

			"por %%mm3, %%mm6		\n\t"

			MOVNTQ" %%mm6, 16(%1)		\n\t"

			"movq 8 (%5, %0, 2), %%mm6;" 

			"pxor %%mm4, %%mm4		\n\t"



#else



			"pxor %%mm4, %%mm4		\n\t"

			"movq %%mm0, %%mm5		\n\t" 

			"movq %%mm1, %%mm6		\n\t" 

			"punpcklbw %%mm2, %%mm0		\n\t" 

			"punpcklbw %%mm4, %%mm1		\n\t" 

			"punpckhbw %%mm2, %%mm5		\n\t" 

			"punpckhbw %%mm4, %%mm6		\n\t" 

			"movq %%mm0, %%mm7		\n\t" 

			"movq %%mm5, %%mm3		\n\t" 

			"punpcklwd %%mm1, %%mm7		\n\t" 

			"punpckhwd %%mm1, %%mm0		\n\t" 

			"punpcklwd %%mm6, %%mm5		\n\t" 

			"punpckhwd %%mm6, %%mm3		\n\t" 



			"movq %%mm7, %%mm2		\n\t" 

			"movq %%mm0, %%mm6		\n\t" 

			"movq %%mm5, %%mm1		\n\t" 

			"movq %%mm3, %%mm4		\n\t" 



			"psllq $40, %%mm7		\n\t" 

			"psllq $40, %%mm0		\n\t" 

			"psllq $40, %%mm5		\n\t" 

			"psllq $40, %%mm3		\n\t" 



			"punpckhdq %%mm2, %%mm7		\n\t" 

			"punpckhdq %%mm6, %%mm0		\n\t" 

			"punpckhdq %%mm1, %%mm5		\n\t" 

			"punpckhdq %%mm4, %%mm3		\n\t" 



			"psrlq $8, %%mm7		\n\t" 

			"movq %%mm0, %%mm6		\n\t" 

			"psllq $40, %%mm0		\n\t" 

			"por %%mm0, %%mm7		\n\t" 

			MOVNTQ" %%mm7, (%1)		\n\t"



			"movd 4 (%2, %0), %%mm0;" 



			"psrlq $24, %%mm6		\n\t" 

			"movq %%mm5, %%mm1		\n\t" 

			"psllq $24, %%mm5		\n\t" 

			"por %%mm5, %%mm6		\n\t" 

			MOVNTQ" %%mm6, 8(%1)		\n\t"



			"movq 8 (%5, %0, 2), %%mm6;" 



			"psrlq $40, %%mm1		\n\t" 

			"psllq $8, %%mm3		\n\t" 

			"por %%mm3, %%mm1		\n\t" 

			MOVNTQ" %%mm1, 16(%1)		\n\t"



			"movd 4 (%3, %0), %%mm1;" 

			"pxor %%mm4, %%mm4		\n\t"

#endif



		     "add $24, %1			\n\t"

		     "add $4, %0			\n\t"

		     " js 1b				\n\t"



		     : "+r" (index), "+r" (_image)

		     : "r" (_pu - index), "r" (_pv - index), "r"(&c->redDither), "r" (_py - 2*index)

		     );

    }



    __asm__ __volatile__ (EMMS);

    return srcSliceH;

}