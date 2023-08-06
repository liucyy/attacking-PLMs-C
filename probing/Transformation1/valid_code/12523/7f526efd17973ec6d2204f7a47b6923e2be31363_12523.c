void palette8tobgr32(const uint8_t *src, uint8_t *dst, unsigned num_pixels, const uint8_t *palette)

{

	unsigned i;

	for(i=0; i<num_pixels; i++)

	{

		#ifdef WORDS_BIGENDIAN

		dst[3]= palette[ src[i]*4+0 ];

		dst[2]= palette[ src[i]*4+1 ];

		dst[1]= palette[ src[i]*4+2 ];

		#else

		

		dst[0]= palette[ src[i]*4+0 ];

		dst[1]= palette[ src[i]*4+1 ];

		dst[2]= palette[ src[i]*4+2 ];



		#endif

		

		dst+= 4;

	}

}