static inline int RENAME(yuv420_rgb32)(SwsContext *c, uint8_t* src[], int srcStride[], int srcSliceY,

             int srcSliceH, uint8_t* dst[], int dstStride[]){

    int y, h_size;



    if(c->srcFormat == PIX_FMT_YUV422P){

	srcStride[1] *= 2;

	srcStride[2] *= 2;

    }



    h_size= (c->dstW+7)&~7;

    if(h_size*4 > FFABS(dstStride[0])) h_size-=8;



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

		     

		     "pxor %%mm3, %%mm3;" 



		     "movq %%mm0, %%mm6;" 

		     "movq %%mm1, %%mm7;" 



		     "movq %%mm0, %%mm4;" 

		     "movq %%mm1, %%mm5;" 



		     "punpcklbw %%mm2, %%mm6;" 

		     "punpcklbw %%mm3, %%mm7;" 



		     "punpcklwd %%mm7, %%mm6;" 

		     MOVNTQ " %%mm6, (%1);" 



		     "movq %%mm0, %%mm6;" 

		     "punpcklbw %%mm2, %%mm6;" 



		     "punpckhwd %%mm7, %%mm6;" 

		     MOVNTQ " %%mm6, 8 (%1);" 



		     "punpckhbw %%mm2, %%mm4;" 

		     "punpckhbw %%mm3, %%mm5;" 



		     "punpcklwd %%mm5, %%mm4;" 

		     MOVNTQ " %%mm4, 16 (%1);" 



		     "movq %%mm0, %%mm4;" 

		     "punpckhbw %%mm2, %%mm4;" 



		     "punpckhwd %%mm5, %%mm4;" 

		     MOVNTQ " %%mm4, 24 (%1);" 



		     "movd 4 (%2, %0), %%mm0;" 

		     "movd 4 (%3, %0), %%mm1;" 



		     "pxor %%mm4, %%mm4;" 

		     "movq 8 (%5, %0, 2), %%mm6;" 



		     "add $32, %1			\n\t"

		     "add $4, %0			\n\t"

		     " js 1b				\n\t"



		     : "+r" (index), "+r" (_image)

		     : "r" (_pu - index), "r" (_pv - index), "r"(&c->redDither), "r" (_py - 2*index)

		     );

    }



    __asm__ __volatile__ (EMMS);

    return srcSliceH;

}