static inline void RENAME(hyscale)(uint16_t *dst, long dstWidth, uint8_t *src, int srcW, int xInc,

				   int flags, int canMMX2BeUsed, int16_t *hLumFilter,

				   int16_t *hLumFilterPos, int hLumFilterSize, void *funnyYCode, 

				   int srcFormat, uint8_t *formatConvBuffer, int16_t *mmx2Filter,

				   int32_t *mmx2FilterPos)

{

    if(srcFormat==IMGFMT_YUY2)

    {

	RENAME(yuy2ToY)(formatConvBuffer, src, srcW);

	src= formatConvBuffer;

    }

    else if(srcFormat==IMGFMT_UYVY)

    {

	RENAME(uyvyToY)(formatConvBuffer, src, srcW);

	src= formatConvBuffer;

    }

    else if(srcFormat==IMGFMT_BGR32)

    {

	RENAME(bgr32ToY)(formatConvBuffer, src, srcW);

	src= formatConvBuffer;

    }

    else if(srcFormat==IMGFMT_BGR24)

    {

	RENAME(bgr24ToY)(formatConvBuffer, src, srcW);

	src= formatConvBuffer;

    }

    else if(srcFormat==IMGFMT_BGR16)

    {

	RENAME(bgr16ToY)(formatConvBuffer, src, srcW);

	src= formatConvBuffer;

    }

    else if(srcFormat==IMGFMT_BGR15)

    {

	RENAME(bgr15ToY)(formatConvBuffer, src, srcW);

	src= formatConvBuffer;

    }

    else if(srcFormat==IMGFMT_RGB32)

    {

	RENAME(rgb32ToY)(formatConvBuffer, src, srcW);

	src= formatConvBuffer;

    }

    else if(srcFormat==IMGFMT_RGB24)

    {

	RENAME(rgb24ToY)(formatConvBuffer, src, srcW);

	src= formatConvBuffer;

    }



#ifdef HAVE_MMX

	

    if(!(flags&SWS_FAST_BILINEAR) || (!canMMX2BeUsed))

#else

    if(!(flags&SWS_FAST_BILINEAR))

#endif

    {

    	RENAME(hScale)(dst, dstWidth, src, srcW, xInc, hLumFilter, hLumFilterPos, hLumFilterSize);

    }

    else 

    {

#if defined(ARCH_X86) || defined(ARCH_X86_64)

#ifdef HAVE_MMX2

	int i;

	if(canMMX2BeUsed)

	{

		asm volatile(

			"pxor %%mm7, %%mm7		\n\t"

			"mov %0, %%"REG_c"		\n\t"

			"mov %1, %%"REG_D"		\n\t"

			"mov %2, %%"REG_d"		\n\t"

			"mov %3, %%"REG_b"		\n\t"

			"xor %%"REG_a", %%"REG_a"	\n\t" 

			PREFETCH" (%%"REG_c")		\n\t"

			PREFETCH" 32(%%"REG_c")		\n\t"

			PREFETCH" 64(%%"REG_c")		\n\t"



#ifdef ARCH_X86_64



#define FUNNY_Y_CODE \

			"movl (%%"REG_b"), %%esi	\n\t"\

			"call *%4			\n\t"\

			"movl (%%"REG_b", %%"REG_a"), %%esi\n\t"\

			"add %%"REG_S", %%"REG_c"	\n\t"\

			"add %%"REG_a", %%"REG_D"	\n\t"\

			"xor %%"REG_a", %%"REG_a"	\n\t"\



#else



#define FUNNY_Y_CODE \

			"movl (%%"REG_b"), %%esi	\n\t"\

			"call *%4			\n\t"\

			"addl (%%"REG_b", %%"REG_a"), %%"REG_c"\n\t"\

			"add %%"REG_a", %%"REG_D"	\n\t"\

			"xor %%"REG_a", %%"REG_a"	\n\t"\



#endif



FUNNY_Y_CODE

FUNNY_Y_CODE

FUNNY_Y_CODE

FUNNY_Y_CODE

FUNNY_Y_CODE

FUNNY_Y_CODE

FUNNY_Y_CODE

FUNNY_Y_CODE



			:: "m" (src), "m" (dst), "m" (mmx2Filter), "m" (mmx2FilterPos),

			"m" (funnyYCode)

			: "%"REG_a, "%"REG_b, "%"REG_c, "%"REG_d, "%"REG_S, "%"REG_D

		);

		for(i=dstWidth-1; (i*xInc)>>16 >=srcW-1; i--) dst[i] = src[srcW-1]*128;

	}

	else

	{

#endif

	long xInc_shr16 = xInc >> 16;

	uint16_t xInc_mask = xInc & 0xffff;

	

	asm volatile(

		"xor %%"REG_a", %%"REG_a"	\n\t" 

		"xor %%"REG_b", %%"REG_b"	\n\t" 

		"xorl %%ecx, %%ecx		\n\t" 

		ASMALIGN16

		"1:				\n\t"

		"movzbl  (%0, %%"REG_b"), %%edi	\n\t" 

		"movzbl 1(%0, %%"REG_b"), %%esi	\n\t" 

		"subl %%edi, %%esi		\n\t" 

		"imull %%ecx, %%esi		\n\t" 

		"shll $16, %%edi		\n\t"

		"addl %%edi, %%esi		\n\t" 

		"mov %1, %%"REG_D"		\n\t"

		"shrl $9, %%esi			\n\t"

		"movw %%si, (%%"REG_D", %%"REG_a", 2)\n\t"

		"addw %4, %%cx			\n\t" 

		"adc %3, %%"REG_b"		\n\t" 



		"movzbl (%0, %%"REG_b"), %%edi	\n\t" 

		"movzbl 1(%0, %%"REG_b"), %%esi	\n\t" 

		"subl %%edi, %%esi		\n\t" 

		"imull %%ecx, %%esi		\n\t" 

		"shll $16, %%edi		\n\t"

		"addl %%edi, %%esi		\n\t" 

		"mov %1, %%"REG_D"		\n\t"

		"shrl $9, %%esi			\n\t"

		"movw %%si, 2(%%"REG_D", %%"REG_a", 2)\n\t"

		"addw %4, %%cx			\n\t" 

		"adc %3, %%"REG_b"		\n\t" 





		"add $2, %%"REG_a"		\n\t"

		"cmp %2, %%"REG_a"		\n\t"

		" jb 1b				\n\t"





		:: "r" (src), "m" (dst), "m" (dstWidth), "m" (xInc_shr16), "m" (xInc_mask)

		: "%"REG_a, "%"REG_b, "%ecx", "%"REG_D, "%esi"

		);

#ifdef HAVE_MMX2

	} 

#endif

#else

	int i;

	unsigned int xpos=0;

	for(i=0;i<dstWidth;i++)

	{

		register unsigned int xx=xpos>>16;

		register unsigned int xalpha=(xpos&0xFFFF)>>9;

		dst[i]= (src[xx]<<7) + (src[xx+1] - src[xx])*xalpha;

		xpos+=xInc;

	}

#endif

    }

}