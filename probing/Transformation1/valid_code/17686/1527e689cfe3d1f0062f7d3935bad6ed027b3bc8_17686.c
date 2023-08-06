static void draw_slice(AVFilterLink *inlink, int y, int h, int slice_dir)

{

    FadeContext *fade = inlink->dst->priv;

    AVFilterBufferRef *outpic = inlink->cur_buf;

    uint8_t *p;

    int i, j, plane;



    if (fade->factor < 65536) {

        

        for (i = 0; i < h; i++) {

            p = outpic->data[0] + (y+i) * outpic->linesize[0];

            for (j = 0; j < inlink->w * fade->bpp; j++) {

                

                *p = (*p * fade->factor + 32768) >> 16;

                p++;

            }

        }



        if (outpic->data[0] && outpic->data[1]) {

            

            for (plane = 1; plane < 3; plane++) {

                for (i = 0; i < h; i++) {

                    p = outpic->data[plane] + ((y+i) >> fade->vsub) * outpic->linesize[plane];

                    for (j = 0; j < inlink->w >> fade->hsub; j++) {

                        

                        *p = ((*p - 128) * fade->factor + 8421367) >> 16;

                        p++;

                    }

                }

            }

        }

    }



    avfilter_draw_slice(inlink->dst->outputs[0], y, h, slice_dir);

}