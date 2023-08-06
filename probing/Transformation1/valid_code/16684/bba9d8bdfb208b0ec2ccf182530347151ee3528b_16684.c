static void qpeg_decode_inter(QpegContext *qctx, uint8_t *dst,

                              int stride, int width, int height,

                              int delta, const uint8_t *ctable,

                              uint8_t *refdata)

{

    int i, j;

    int code;

    int filled = 0;

    int orig_height;



    

    for(i = 0; i < height; i++)

        memcpy(refdata + (i * width), dst + (i * stride), width);



    orig_height = height;

    height--;

    dst = dst + height * stride;



    while ((bytestream2_get_bytes_left(&qctx->buffer) > 0) && (height >= 0)) {

        code = bytestream2_get_byte(&qctx->buffer);



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



                    

                    corr = bytestream2_get_byte(&qctx->buffer);



                    val = corr >> 4;

                    if(val > 7)

                        val -= 16;

                    me_x = val;



                    val = corr & 0xF;

                    if(val > 7)

                        val -= 16;

                    me_y = val;



                    

                    if ((me_x + filled < 0) || (me_x + me_w + filled > width) ||

                       (height - me_y - me_h < 0) || (height - me_y > orig_height) ||

                       (filled + me_w > width) || (height - me_h < 0))

                        av_log(NULL, AV_LOG_ERROR, "Bogus motion vector (%i,%i), block size %ix%i at %i,%i\n",

                               me_x, me_y, me_w, me_h, filled, height);

                    else {

                        

                        me_plane = refdata + (filled + me_x) + (height - me_y) * width;

                        for(j = 0; j < me_h; j++) {

                            for(i = 0; i < me_w; i++)

                                dst[filled + i - (j * stride)] = me_plane[i - (j * width)];

                        }

                    }

                }

                code = bytestream2_get_byte(&qctx->buffer);

            }

        }



        if(code == 0xE0) 

            break;

        if(code > 0xE0) { 

            int p;



            code &= 0x1F;

            p = bytestream2_get_byte(&qctx->buffer);

            for(i = 0; i <= code; i++) {

                dst[filled++] = p;

                if(filled >= width) {

                    filled = 0;

                    dst -= stride;

                    height--;

                    if (height < 0)

                        break;

                }

            }

        } else if(code >= 0xC0) { 

            code &= 0x1F;



            for(i = 0; i <= code; i++) {

                dst[filled++] = bytestream2_get_byte(&qctx->buffer);

                if(filled >= width) {

                    filled = 0;

                    dst -= stride;

                    height--;

                    if (height < 0)

                        break;

                }

            }

        } else if(code >= 0x80) { 

            int skip;



            code &= 0x3F;

            

            if(!code)

                skip = bytestream2_get_byte(&qctx->buffer) +  64;

            else if(code == 1)

                skip = bytestream2_get_byte(&qctx->buffer) + 320;

            else

                skip = code;

            filled += skip;

            while( filled >= width) {

                filled -= width;

                dst -= stride;

                height--;

                if(height < 0)

                    break;

            }

        } else {

            

            if(code) {

                dst[filled++] = ctable[code & 0x7F];

            }

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