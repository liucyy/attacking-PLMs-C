static int decode_band_hdr(IVI4DecContext *ctx, IVIBandDesc *band,

                           AVCodecContext *avctx)

{

    int plane, band_num, indx, transform_id, scan_indx;

    int i;

    int quant_mat;



    plane    = get_bits(&ctx->gb, 2);

    band_num = get_bits(&ctx->gb, 4);

    if (band->plane != plane || band->band_num != band_num) {

        av_log(avctx, AV_LOG_ERROR, "Invalid band header sequence!\n");

        return AVERROR_INVALIDDATA;

    }



    band->is_empty = get_bits1(&ctx->gb);

    if (!band->is_empty) {

        

        if (get_bits1(&ctx->gb))

            skip_bits(&ctx->gb, 16);



        band->is_halfpel = get_bits(&ctx->gb, 2);

        if (band->is_halfpel >= 2) {

            av_log(avctx, AV_LOG_ERROR, "Invalid/unsupported mv resolution: %d!\n",

                   band->is_halfpel);

            return AVERROR_INVALIDDATA;

        }

#if IVI4_STREAM_ANALYSER

        if (!band->is_halfpel)

            ctx->uses_fullpel = 1;

#endif



        band->checksum_present = get_bits1(&ctx->gb);

        if (band->checksum_present)

            band->checksum = get_bits(&ctx->gb, 16);



        indx = get_bits(&ctx->gb, 2);

        if (indx == 3) {

            av_log(avctx, AV_LOG_ERROR, "Invalid block size!\n");

            return AVERROR_INVALIDDATA;

        }

        band->mb_size  = 16 >> indx;

        band->blk_size = 8 >> (indx >> 1);



        band->inherit_mv     = get_bits1(&ctx->gb);

        band->inherit_qdelta = get_bits1(&ctx->gb);



        band->glob_quant = get_bits(&ctx->gb, 5);



        if (!get_bits1(&ctx->gb) || ctx->frame_type == FRAMETYPE_INTRA) {

            transform_id = get_bits(&ctx->gb, 5);

            if (transform_id >= FF_ARRAY_ELEMS(transforms) ||

                !transforms[transform_id].inv_trans) {

                av_log_ask_for_sample(avctx, "Unimplemented transform: %d!\n", transform_id);

                return AVERROR_PATCHWELCOME;

            }

            if ((transform_id >= 7 && transform_id <= 9) ||

                 transform_id == 17) {

                av_log_ask_for_sample(avctx, "DCT transform not supported yet!\n");

                return AVERROR_PATCHWELCOME;

            }



            if (transform_id < 10 && band->blk_size < 8) {

                av_log(avctx, AV_LOG_ERROR, "wrong transform size!\n");

                return AVERROR_INVALIDDATA;

            }

#if IVI4_STREAM_ANALYSER

            if ((transform_id >= 0 && transform_id <= 2) || transform_id == 10)

                ctx->uses_haar = 1;

#endif



            band->inv_transform = transforms[transform_id].inv_trans;

            band->dc_transform  = transforms[transform_id].dc_trans;

            band->is_2d_trans   = transforms[transform_id].is_2d_trans;

            band->transform_size= (transform_id < 10) ? 8 : 4;



            scan_indx = get_bits(&ctx->gb, 4);

            if ((scan_indx>4 && scan_indx<10) != (band->blk_size==4)) {

                av_log(avctx, AV_LOG_ERROR, "mismatching scan table!\n");

                return AVERROR_INVALIDDATA;

            }

            if (scan_indx == 15) {

                av_log(avctx, AV_LOG_ERROR, "Custom scan pattern encountered!\n");

                return AVERROR_INVALIDDATA;

            }

            band->scan = scan_index_to_tab[scan_indx];



            quant_mat = get_bits(&ctx->gb, 5);

            if (quant_mat == 31) {

                av_log(avctx, AV_LOG_ERROR, "Custom quant matrix encountered!\n");

                return AVERROR_INVALIDDATA;

            }

            if (quant_mat > 21) {

                av_log(avctx, AV_LOG_ERROR, "Invalid quant matrix encountered!\n");

                return AVERROR_INVALIDDATA;

            }

            band->quant_mat = quant_mat;

        }



        

        if (ff_ivi_dec_huff_desc(&ctx->gb, get_bits1(&ctx->gb), IVI_BLK_HUFF,

                                 &band->blk_vlc, avctx))

            return AVERROR_INVALIDDATA;



        

        band->rvmap_sel = get_bits1(&ctx->gb) ? get_bits(&ctx->gb, 3) : 8;



        

        band->num_corr = 0; 

        if (get_bits1(&ctx->gb)) {

            band->num_corr = get_bits(&ctx->gb, 8); 

            if (band->num_corr > 61) {

                av_log(avctx, AV_LOG_ERROR, "Too many corrections: %d\n",

                       band->num_corr);

                return AVERROR_INVALIDDATA;

            }



            

            for (i = 0; i < band->num_corr * 2; i++)

                band->corr[i] = get_bits(&ctx->gb, 8);

        }

    }



    if (band->blk_size == 8) {

        band->intra_base = &ivi4_quant_8x8_intra[quant_index_to_tab[band->quant_mat]][0];

        band->inter_base = &ivi4_quant_8x8_inter[quant_index_to_tab[band->quant_mat]][0];

    } else {

        band->intra_base = &ivi4_quant_4x4_intra[quant_index_to_tab[band->quant_mat]][0];

        band->inter_base = &ivi4_quant_4x4_inter[quant_index_to_tab[band->quant_mat]][0];

    }



    

    band->intra_scale = NULL;

    band->inter_scale = NULL;



    align_get_bits(&ctx->gb);



    if (!band->scan) {

        av_log(avctx, AV_LOG_ERROR, "band->scan not set\n");

        return AVERROR_INVALIDDATA;

    }



    return 0;

}