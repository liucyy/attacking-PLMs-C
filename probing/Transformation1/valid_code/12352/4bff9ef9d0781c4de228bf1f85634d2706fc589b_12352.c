static inline void RENAME(yuy2toyv12)(const uint8_t *src, uint8_t *ydst, uint8_t *udst, uint8_t *vdst,

	long width, long height,

	long lumStride, long chromStride, long srcStride)

{

	long y;

	const long chromWidth= width>>1;

	for(y=0; y<height; y+=2)

	{

#ifdef HAVE_MMX

		asm volatile(

			"xor %%"REG_a", %%"REG_a"	\n\t"

			"pcmpeqw %%mm7, %%mm7		\n\t"

			"psrlw $8, %%mm7		\n\t" 

			ASMALIGN16

			"1:				\n\t"

			PREFETCH" 64(%0, %%"REG_a", 4)	\n\t"

			"movq (%0, %%"REG_a", 4), %%mm0	\n\t" 

			"movq 8(%0, %%"REG_a", 4), %%mm1\n\t" 

			"movq %%mm0, %%mm2		\n\t" 

			"movq %%mm1, %%mm3		\n\t" 

			"psrlw $8, %%mm0		\n\t" 

			"psrlw $8, %%mm1		\n\t" 

			"pand %%mm7, %%mm2		\n\t" 

			"pand %%mm7, %%mm3		\n\t" 

			"packuswb %%mm1, %%mm0		\n\t" 

			"packuswb %%mm3, %%mm2		\n\t" 



			MOVNTQ" %%mm2, (%1, %%"REG_a", 2)\n\t"



			"movq 16(%0, %%"REG_a", 4), %%mm1\n\t" 

			"movq 24(%0, %%"REG_a", 4), %%mm2\n\t" 

			"movq %%mm1, %%mm3		\n\t" 

			"movq %%mm2, %%mm4		\n\t" 

			"psrlw $8, %%mm1		\n\t" 

			"psrlw $8, %%mm2		\n\t" 

			"pand %%mm7, %%mm3		\n\t" 

			"pand %%mm7, %%mm4		\n\t" 

			"packuswb %%mm2, %%mm1		\n\t" 

			"packuswb %%mm4, %%mm3		\n\t" 



			MOVNTQ" %%mm3, 8(%1, %%"REG_a", 2)\n\t"



			"movq %%mm0, %%mm2		\n\t" 

			"movq %%mm1, %%mm3		\n\t" 

			"psrlw $8, %%mm0		\n\t" 

			"psrlw $8, %%mm1		\n\t" 

			"pand %%mm7, %%mm2		\n\t" 

			"pand %%mm7, %%mm3		\n\t" 

			"packuswb %%mm1, %%mm0		\n\t" 

			"packuswb %%mm3, %%mm2		\n\t" 



			MOVNTQ" %%mm0, (%3, %%"REG_a")	\n\t"

			MOVNTQ" %%mm2, (%2, %%"REG_a")	\n\t"



			"add $8, %%"REG_a"		\n\t"

			"cmp %4, %%"REG_a"		\n\t"

			" jb 1b				\n\t"

			::"r"(src), "r"(ydst), "r"(udst), "r"(vdst), "g" (chromWidth)

			: "memory", "%"REG_a

		);



		ydst += lumStride;

		src  += srcStride;



		asm volatile(

			"xor %%"REG_a", %%"REG_a"	\n\t"

			ASMALIGN16

			"1:				\n\t"

			PREFETCH" 64(%0, %%"REG_a", 4)	\n\t"

			"movq (%0, %%"REG_a", 4), %%mm0	\n\t" 

			"movq 8(%0, %%"REG_a", 4), %%mm1\n\t" 

			"movq 16(%0, %%"REG_a", 4), %%mm2\n\t" 

			"movq 24(%0, %%"REG_a", 4), %%mm3\n\t" 

			"pand %%mm7, %%mm0		\n\t" 

			"pand %%mm7, %%mm1		\n\t" 

			"pand %%mm7, %%mm2		\n\t" 

			"pand %%mm7, %%mm3		\n\t" 

			"packuswb %%mm1, %%mm0		\n\t" 

			"packuswb %%mm3, %%mm2		\n\t" 



			MOVNTQ" %%mm0, (%1, %%"REG_a", 2)\n\t"

			MOVNTQ" %%mm2, 8(%1, %%"REG_a", 2)\n\t"



			"add $8, %%"REG_a"		\n\t"

			"cmp %4, %%"REG_a"		\n\t"

			" jb 1b				\n\t"



			::"r"(src), "r"(ydst), "r"(udst), "r"(vdst), "g" (chromWidth)

			: "memory", "%"REG_a

		);

#else

		long i;

		for(i=0; i<chromWidth; i++)

		{

			ydst[2*i+0] 	= src[4*i+0];

			udst[i] 	= src[4*i+1];

			ydst[2*i+1] 	= src[4*i+2];

			vdst[i] 	= src[4*i+3];

		}

		ydst += lumStride;

		src  += srcStride;



		for(i=0; i<chromWidth; i++)

		{

			ydst[2*i+0] 	= src[4*i+0];

			ydst[2*i+1] 	= src[4*i+2];

		}

#endif

		udst += chromStride;

		vdst += chromStride;

		ydst += lumStride;

		src  += srcStride;

	}

#ifdef HAVE_MMX

asm volatile(   EMMS" \n\t"

        	SFENCE" \n\t"

        	:::"memory");

#endif

}