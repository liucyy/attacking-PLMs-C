static void qpeg_decode_inter(uint8_t *src, uint8_t *dst, int size,

			    int stride, int width, int height,

			    int delta, uint8_t *ctable, uint8_t *refdata)

{

    int i, j;

    int code;

    int filled = 0;

    uint8_t *blkdata;

    

    

    for(i = 0; i < height; i++)

	memcpy(refdata + (i * width), dst + (i * stride), width);

    

    blkdata = src - 0x86;

    height--;

    dst = dst + height * stride;



    while(size > 0) {

	code = *src++;

	size--;

	

	if(delta) {

	    

	    while((code & 0xF0) == 0xF0) {

		if(delta == 1) {

		    int me_idx;

		    int me_w, me_h, me_x, me_y;

		    uint8_t *me_plane;

		    int corr, val;

		    

		    

		    me_idx = code & 0xF;

		    me_w = qpeg_table_w[me_idx];

		    me_h = qpeg_table_h[me_idx];

		    

		    

		    corr = *src++;

		    size--;



		    val = corr >> 4;

		    if(val > 7)

			val -= 16;

		    me_x = val;

		    

		    val = corr & 0xF;

		    if(val > 7)

			val -= 16;

		    me_y = val;

		    

		    

		    me_plane = refdata + (filled + me_x) + (height - me_y) * width;

		    for(j = 0; j < me_h; j++) {

			for(i = 0; i < me_w; i++)

			    dst[filled + i - (j * stride)] = me_plane[i - (j * width)];

		    }

		}

		code = *src++;

		size--;

	    }

	}

	

	if(code == 0xE0) 

	    break;

	if(code > 0xE0) { 

	    int p;



	    code &= 0x1F;

	    p = *src++;

	    size--;

	    for(i = 0; i <= code; i++) {

		dst[filled++] = p;

		if(filled >= width) {

		    filled = 0;

		    dst -= stride;

		    height--;

		}

	    }

	} else if(code >= 0xC0) { 

	    code &= 0x1F;

	    

	    for(i = 0; i <= code; i++) {

		dst[filled++] = *src++;

		if(filled >= width) {

		    filled = 0;

		    dst -= stride;

		    height--;

		}

	    }

	    size -= code + 1;

	} else if(code >= 0x80) { 

	    int skip;

	    

	    code &= 0x3F;

	    

	    if(!code)

		skip = (*src++) + 64;

	    else if(code == 1)

		skip = (*src++) + 320;

	    else

		skip = code;

	    filled += skip;

	    while( filled >= width) {

		filled -= width;

		dst -= stride;

		height--;

	    }

	} else {

	    

	    if(code)

		dst[filled++] = ctable[code & 0x7F];

	    else

		filled++;

	    if(filled >= width) {

		filled = 0;

		dst -= stride;

		height--;

	    }

	}

    }

}