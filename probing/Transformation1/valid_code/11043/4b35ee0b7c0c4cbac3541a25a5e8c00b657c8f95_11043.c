void ff_ivi_process_empty_tile(AVCodecContext *avctx, IVIBandDesc *band,

                               IVITile *tile, int32_t mv_scale)

{

    int             x, y, need_mc, mbn, blk, num_blocks, mv_x, mv_y, mc_type;

    int             offs, mb_offset, row_offset;

    IVIMbInfo       *mb, *ref_mb;

    const int16_t   *src;

    int16_t         *dst;

    void (*mc_no_delta_func)(int16_t *buf, const int16_t *ref_buf, uint32_t pitch,

                             int mc_type);



    offs       = tile->ypos * band->pitch + tile->xpos;

    mb         = tile->mbs;

    ref_mb     = tile->ref_mbs;

    row_offset = band->mb_size * band->pitch;

    need_mc    = 0; 



    for (y = tile->ypos; y < (tile->ypos + tile->height); y += band->mb_size) {

        mb_offset = offs;



        for (x = tile->xpos; x < (tile->xpos + tile->width); x += band->mb_size) {

            mb->xpos     = x;

            mb->ypos     = y;

            mb->buf_offs = mb_offset;



            mb->type = 1; 

            mb->cbp  = 0; 



            if (!band->qdelta_present && !band->plane && !band->band_num) {

                mb->q_delta = band->glob_quant;

                mb->mv_x    = 0;

                mb->mv_y    = 0;

            }



            if (band->inherit_qdelta && ref_mb)

                mb->q_delta = ref_mb->q_delta;



            if (band->inherit_mv) {

                

                if (mv_scale) {

                    mb->mv_x = ivi_scale_mv(ref_mb->mv_x, mv_scale);

                    mb->mv_y = ivi_scale_mv(ref_mb->mv_y, mv_scale);

                } else {

                    mb->mv_x = ref_mb->mv_x;

                    mb->mv_y = ref_mb->mv_y;

                }

                need_mc |= mb->mv_x || mb->mv_y; 

            }



            mb++;

            if (ref_mb)

                ref_mb++;

            mb_offset += band->mb_size;

        } 

        offs += row_offset;

    } 



    if (band->inherit_mv && need_mc) { 

        num_blocks = (band->mb_size != band->blk_size) ? 4 : 1; 

        mc_no_delta_func = (band->blk_size == 8) ? ff_ivi_mc_8x8_no_delta

                                                 : ff_ivi_mc_4x4_no_delta;



        for (mbn = 0, mb = tile->mbs; mbn < tile->num_MBs; mb++, mbn++) {

            mv_x = mb->mv_x;

            mv_y = mb->mv_y;

            if (!band->is_halfpel) {

                mc_type = 0; 

            } else {

                mc_type = ((mv_y & 1) << 1) | (mv_x & 1);

                mv_x >>= 1;

                mv_y >>= 1; 

            }



            for (blk = 0; blk < num_blocks; blk++) {

                

                offs = mb->buf_offs + band->blk_size * ((blk & 1) + !!(blk & 2) * band->pitch);

                mc_no_delta_func(band->buf + offs,

                                 band->ref_buf + offs + mv_y * band->pitch + mv_x,

                                 band->pitch, mc_type);

            }

        }

    } else {

        

        src = band->ref_buf + tile->ypos * band->pitch + tile->xpos;

        dst = band->buf     + tile->ypos * band->pitch + tile->xpos;

        for (y = 0; y < tile->height; y++) {

            memcpy(dst, src, tile->width*sizeof(band->buf[0]));

            src += band->pitch;

            dst += band->pitch;

        }

    }

}