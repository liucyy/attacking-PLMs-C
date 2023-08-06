static int mc_subpel(DiracContext *s, DiracBlock *block, const uint8_t *src[5],

                     int x, int y, int ref, int plane)

{

    Plane *p = &s->plane[plane];

    uint8_t **ref_hpel = s->ref_pics[ref]->hpel[plane];

    int motion_x = block->u.mv[ref][0];

    int motion_y = block->u.mv[ref][1];

    int mx, my, i, epel, nplanes = 0;



    if (plane) {

        motion_x >>= s->chroma_x_shift;

        motion_y >>= s->chroma_y_shift;

    }



    mx         = motion_x & ~(-1 << s->mv_precision);

    my         = motion_y & ~(-1 << s->mv_precision);

    motion_x >>= s->mv_precision;

    motion_y >>= s->mv_precision;

    

    

    mx      <<= 3 - s->mv_precision;

    my      <<= 3 - s->mv_precision;



    x += motion_x;

    y += motion_y;

    epel = (mx|my)&1;



    

    if (!((mx|my)&3)) {

        nplanes = 1;

        src[0] = ref_hpel[(my>>1)+(mx>>2)] + y*p->stride + x;

    } else {

        

        nplanes = 4;

        for (i = 0; i < 4; i++)

            src[i] = ref_hpel[i] + y*p->stride + x;



        

        if (mx > 4) {

            src[0] += 1;

            src[2] += 1;

            x++;

        }

        if (my > 4) {

            src[0] += p->stride;

            src[1] += p->stride;

            y++;

        }



        

        if (!epel) {

            

            if (!(mx&3)) {

                

                src[!mx] = src[2 + !!mx];

                nplanes = 2;

            } else if (!(my&3)) {

                src[0] = src[(my>>1)  ];

                src[1] = src[(my>>1)+1];

                nplanes = 2;

            }

        } else {

            

            if (mx > 4) {

                FFSWAP(const uint8_t *, src[0], src[1]);

                FFSWAP(const uint8_t *, src[2], src[3]);

            }

            if (my > 4) {

                FFSWAP(const uint8_t *, src[0], src[2]);

                FFSWAP(const uint8_t *, src[1], src[3]);

            }

            src[4] = epel_weights[my&3][mx&3];

        }

    }



    

    if (x + p->xblen > p->width +EDGE_WIDTH/2 ||

        y + p->yblen > p->height+EDGE_WIDTH/2 ||

        x < 0 || y < 0) {

        for (i = 0; i < nplanes; i++) {

            ff_emulated_edge_mc(s->edge_emu_buffer[i], src[i],

                                p->stride, p->stride,

                                p->xblen, p->yblen, x, y,

                                p->width+EDGE_WIDTH/2, p->height+EDGE_WIDTH/2);

            src[i] = s->edge_emu_buffer[i];

        }

    }

    return (nplanes>>1) + epel;

}