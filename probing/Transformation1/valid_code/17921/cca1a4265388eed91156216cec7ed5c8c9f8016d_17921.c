static int decode_rle(CamtasiaContext *c)

{

    unsigned char *src = c->decomp_buf;

    unsigned char *output;

    int p1, p2, line=c->height, pos=0, i;

    

    output = c->pic.data[0] + (c->height - 1) * c->pic.linesize[0];

    while(src < c->decomp_buf + c->decomp_size) {

        p1 = *src++;

        if(p1 == 0) { 

            p2 = *src++;

            if(p2 == 0) { 

                output = c->pic.data[0] + (--line) * c->pic.linesize[0];

                pos = 0;

                continue;

            } else if(p2 == 1) { 

                return 0;

            } else if(p2 == 2) { 

                p1 = *src++;

                p2 = *src++;

                line -= p2;

                pos += p1;

                output = c->pic.data[0] + line * c->pic.linesize[0] + pos * (c->bpp / 8);

                continue;

            }

            

            for(i = 0; i < p2 * (c->bpp / 8); i++) {

                *output++ = *src++;

            }

	    

	    if(c->bpp == 8 && (p2 & 1)) {

		src++;

	    }

            pos += p2;

        } else { 

            int pix[3]; 

            switch(c->bpp){

            case  8: pix[0] = *src++;

                     break;

            case 16: pix[0] = *src++;

                     pix[1] = *src++;

                     break;

            case 24: pix[0] = *src++;

                     pix[1] = *src++;

                     pix[2] = *src++;

                     break;

            }

            for(i = 0; i < p1; i++) {

                switch(c->bpp){

                case  8: *output++ = pix[0];

                         break;

                case 16: *output++ = pix[0];

                         *output++ = pix[1];

                         break;

                case 24: *output++ = pix[0];

                         *output++ = pix[1];

                         *output++ = pix[2];

                         break;

                }

            }

            pos += p1;

        }

    }

    

    av_log(c->avctx, AV_LOG_ERROR, "Camtasia warning: no End-of-picture code\n");        

    return 1;

}