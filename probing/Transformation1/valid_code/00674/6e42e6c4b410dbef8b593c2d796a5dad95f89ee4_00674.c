void palette8tobgr24(const uint8_t *src, uint8_t *dst, long num_pixels, const uint8_t *palette)

{

	long i;



	for(i=0; i<num_pixels; i++)

	{

		

		dst[0]= palette[ src[i]*4+0 ];

		dst[1]= palette[ src[i]*4+1 ];

		dst[2]= palette[ src[i]*4+2 ];

		dst+= 3;

	}

}