static inline int RENAME(yuv420_rgb16)(SwsContext *c, uint8_t* src[], int srcStride[], int srcSliceY,

             int srcSliceH, uint8_t* dst[], int dstStride[]){

    int y, h_size;



    if(c->srcFormat == PIX_FMT_YUV422P){

	srcStride[1] *= 2;

	srcStride[2] *= 2;

    }



    h_size= (c->dstW+7)&~7;

    if(h_size*2 > dstStride[0]) h_size-=8;

    

    __asm__ __volatile__ ("pxor %mm4, %mm4;"  );





    for (y= 0; y<srcSliceH; y++ ) {

	uint8_t *_image = dst[0] + (y+srcSliceY)*dstStride[0];

	uint8_t *_py = src[0] + y*srcStride[0];

	uint8_t *_pu = src[1] + (y>>1)*srcStride[1];

	uint8_t *_pv = src[2] + (y>>1)*srcStride[2];

	long index= -h_size/2;



	b5Dither= dither8[y&1];

	g6Dither= dither4[y&1];

	g5Dither= dither8[y&1];

	r5Dither= dither8[(y+1)&1];

	    

	    __asm__ __volatile__ (

	

		     "movd (%2, %0), %%mm0;" 

		     "movd (%3, %0), %%mm1;" 

		     "movq (%5, %0, 2), %%mm6;" 



		    "1:				\n\t"



YUV2RGB



#ifdef DITHER1XBPP

			"paddusb "MANGLE(b5Dither)", %%mm0;"

			"paddusb "MANGLE(g6Dither)", %%mm2;"

			"paddusb "MANGLE(r5Dither)", %%mm1;"

#endif

		     

		     "pand "MANGLE(mmx_redmask)", %%mm0;" 

		     "pand "MANGLE(mmx_grnmask)", %%mm2;" 

		     "pand "MANGLE(mmx_redmask)", %%mm1;" 



		     "psrlw $3,%%mm0;" 

		     "pxor %%mm4, %%mm4;" 



		     "movq %%mm0, %%mm5;" 

		     "movq %%mm2, %%mm7;" 



		     

		     "punpcklbw %%mm4, %%mm2;" 

		     "punpcklbw %%mm1, %%mm0;" 



		     "psllw $3, %%mm2;" 

		     "por %%mm2, %%mm0;" 



		     "movq 8 (%5, %0, 2), %%mm6;" 

		     MOVNTQ " %%mm0, (%1);" 



		     

		     "punpckhbw %%mm4, %%mm7;" 

		     "punpckhbw %%mm1, %%mm5;" 



		     "psllw $3, %%mm7;" 

		     "movd 4 (%2, %0), %%mm0;" 



		     "por %%mm7, %%mm5;" 

		     "movd 4 (%3, %0), %%mm1;" 



		     MOVNTQ " %%mm5, 8 (%1);" 

		     

		     "add $16, %1			\n\t"

		     "add $4, %0			\n\t"

		     " js 1b				\n\t"

		     

		     : "+r" (index), "+r" (_image)

		     : "r" (_pu - index), "r" (_pv - index), "r"(&c->redDither), "r" (_py - 2*index)

		     );

    }



    __asm__ __volatile__ (EMMS);

    

    return srcSliceH;

}