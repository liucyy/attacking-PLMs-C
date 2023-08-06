int ff_ivi_decode_blocks(GetBitContext *gb, IVIBandDesc *band, IVITile *tile)

{

    int         mbn, blk, num_blocks, num_coeffs, blk_size, scan_pos, run, val,

                pos, is_intra, mc_type, mv_x, mv_y, col_mask;

    uint8_t     col_flags[8];

    int32_t     prev_dc, trvec[64];

    uint32_t    cbp, sym, lo, hi, quant, buf_offs, q;

    IVIMbInfo   *mb;

    RVMapDesc   *rvmap = band->rv_map;

    void (*mc_with_delta_func)(int16_t *buf, const int16_t *ref_buf, uint32_t pitch, int mc_type);

    void (*mc_no_delta_func)  (int16_t *buf, const int16_t *ref_buf, uint32_t pitch, int mc_type);

    const uint16_t  *base_tab;

    const uint8_t   *scale_tab;



    prev_dc = 0; 



    blk_size   = band->blk_size;

    col_mask   = blk_size - 1; 

    num_blocks = (band->mb_size != blk_size) ? 4 : 1; 

    num_coeffs = blk_size * blk_size;

    if (blk_size == 8) {

        mc_with_delta_func = ff_ivi_mc_8x8_delta;

        mc_no_delta_func   = ff_ivi_mc_8x8_no_delta;

    } else {

        mc_with_delta_func = ff_ivi_mc_4x4_delta;

        mc_no_delta_func   = ff_ivi_mc_4x4_no_delta;

    }



    for (mbn = 0, mb = tile->mbs; mbn < tile->num_MBs; mb++, mbn++) {

        is_intra = !mb->type;

        cbp      = mb->cbp;

        buf_offs = mb->buf_offs;



        quant = av_clip(band->glob_quant + mb->q_delta, 0, 23);



        base_tab  = is_intra ? band->intra_base  : band->inter_base;

        scale_tab = is_intra ? band->intra_scale : band->inter_scale;

        if (scale_tab)

            quant = scale_tab[quant];



        if (!is_intra) {

            mv_x = mb->mv_x;

            mv_y = mb->mv_y;

            if (!band->is_halfpel) {

                mc_type = 0; 

            } else {

                mc_type = ((mv_y & 1) << 1) | (mv_x & 1);

                mv_x >>= 1;

                mv_y >>= 1; 

            }

        }



        for (blk = 0; blk < num_blocks; blk++) {

            

            if (blk & 1) {

                buf_offs += blk_size;

            } else if (blk == 2) {

                buf_offs -= blk_size;

                buf_offs += blk_size * band->pitch;

            }



            if (cbp & 1) { 

                scan_pos = -1;

                memset(trvec, 0, num_coeffs*sizeof(trvec[0])); 

                memset(col_flags, 0, sizeof(col_flags));      



                while (scan_pos <= num_coeffs) {

                    sym = get_vlc2(gb, band->blk_vlc.tab->table, IVI_VLC_BITS, 1);

                    if (sym == rvmap->eob_sym)

                        break; 



                    if (sym == rvmap->esc_sym) { 

                        run = get_vlc2(gb, band->blk_vlc.tab->table, IVI_VLC_BITS, 1) + 1;

                        lo  = get_vlc2(gb, band->blk_vlc.tab->table, IVI_VLC_BITS, 1);

                        hi  = get_vlc2(gb, band->blk_vlc.tab->table, IVI_VLC_BITS, 1);

                        val = IVI_TOSIGNED((hi << 6) | lo); 

                    } else {

                        if (sym >= 256U) {

                            av_log(NULL, AV_LOG_ERROR, "Invalid sym encountered: %d.\n", sym);

                            return -1;

                        }

                        run = rvmap->runtab[sym];

                        val = rvmap->valtab[sym];

                    }



                    

                    scan_pos += run;

                    if (scan_pos >= num_coeffs)

                        break;

                    pos = band->scan[scan_pos];



                    if (!val)

                        av_dlog(NULL, "Val = 0 encountered!\n");



                    q = (base_tab[pos] * quant) >> 9;

                    if (q > 1)

                        val = val * q + FFSIGN(val) * (((q ^ 1) - 1) >> 1);

                    trvec[pos] = val;

                    col_flags[pos & col_mask] |= !!val; 

                }



                if (scan_pos >= num_coeffs && sym != rvmap->eob_sym)

                    return -1; 



                

                if (is_intra && band->is_2d_trans) {

                    prev_dc      += trvec[0];

                    trvec[0]      = prev_dc;

                    col_flags[0] |= !!prev_dc;

                }



                

                band->inv_transform(trvec, band->buf + buf_offs,

                                    band->pitch, col_flags);



                

                if (!is_intra)

                    mc_with_delta_func(band->buf + buf_offs,

                                       band->ref_buf + buf_offs + mv_y * band->pitch + mv_x,

                                       band->pitch, mc_type);

            } else {

                

                

                

                if (is_intra && band->dc_transform) {

                    band->dc_transform(&prev_dc, band->buf + buf_offs,

                                       band->pitch, blk_size);

                } else

                    mc_no_delta_func(band->buf + buf_offs,

                                     band->ref_buf + buf_offs + mv_y * band->pitch + mv_x,

                                     band->pitch, mc_type);

            }



            cbp >>= 1;

        }

    }



    align_get_bits(gb);



    return 0;

}