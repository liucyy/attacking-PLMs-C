static inline void RENAME(hyscale)(uint16_t *dst, int dstWidth, uint8_t *src, int srcW, int xInc,

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

#ifdef ARCH_X86

#ifdef HAVE_MMX2

	int i;

	if(canMMX2BeUsed)

	{

		asm volatile(

			"pxor %%mm7, %%mm7		\n\t"

			"movl %0, %%ecx			\n\t"

			"movl %1, %%edi			\n\t"

			"movl %2, %%edx			\n\t"

			"movl %3, %%ebx			\n\t"

			"xorl %%eax, %%eax		\n\t" 

			PREFETCH" (%%ecx)		\n\t"

			PREFETCH" 32(%%ecx)		\n\t"

			PREFETCH" 64(%%ecx)		\n\t"



#define FUNNY_Y_CODE \

			"movl (%%ebx), %%esi		\n\t"\

			"call *%4			\n\t"\

			"addl (%%ebx, %%eax), %%ecx	\n\t"\

			"addl %%eax, %%edi		\n\t"\

			"xorl %%eax, %%eax		\n\t"\



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

			: "%eax", "%ebx", "%ecx", "%edx", "%esi", "%edi"

		);

		for(i=dstWidth-1; (i*xInc)>>16 >=srcW-1; i--) dst[i] = src[srcW-1]*128;

	}

	else

	{

#endif

	

	asm volatile(

		"xorl %%eax, %%eax		\n\t" 

		"xorl %%ebx, %%ebx		\n\t" 

		"xorl %%ecx, %%ecx		\n\t" 

		".balign 16			\n\t"

		"1:				\n\t"

		"movzbl  (%0, %%ebx), %%edi	\n\t" 

		"movzbl 1(%0, %%ebx), %%esi	\n\t" 

		"subl %%edi, %%esi		\n\t" 

		"imull %%ecx, %%esi		\n\t" 

		"shll $16, %%edi		\n\t"

		"addl %%edi, %%esi		\n\t" 

		"movl %1, %%edi			\n\t"

		"shrl $9, %%esi			\n\t"

		"movw %%si, (%%edi, %%eax, 2)	\n\t"

		"addw %4, %%cx			\n\t" 

		"adcl %3, %%ebx			\n\t" 



		"movzbl (%0, %%ebx), %%edi	\n\t" 

		"movzbl 1(%0, %%ebx), %%esi	\n\t" 

		"subl %%edi, %%esi		\n\t" 

		"imull %%ecx, %%esi		\n\t" 

		"shll $16, %%edi		\n\t"

		"addl %%edi, %%esi		\n\t" 

		"movl %1, %%edi			\n\t"

		"shrl $9, %%esi			\n\t"

		"movw %%si, 2(%%edi, %%eax, 2)	\n\t"

		"addw %4, %%cx			\n\t" 

		"adcl %3, %%ebx			\n\t" 





		"addl $2, %%eax			\n\t"

		"cmpl %2, %%eax			\n\t"

		" jb 1b				\n\t"





		:: "r" (src), "m" (dst), "m" (dstWidth), "m" (xInc>>16), "m" (xInc&0xFFFF)

		: "%eax", "%ebx", "%ecx", "%edi", "%esi"

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