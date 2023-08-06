static void decode_component(DiracContext *s, int comp)

{

    AVCodecContext *avctx = s->avctx;

    SubBand *bands[3*MAX_DWT_LEVELS+1];

    enum dirac_subband orientation;

    int level, num_bands = 0;



    

    for (level = 0; level < s->wavelet_depth; level++) {

        for (orientation = !!level; orientation < 4; orientation++) {

            SubBand *b = &s->plane[comp].band[level][orientation];

            bands[num_bands++] = b;



            align_get_bits(&s->gb);

            

            b->length = svq3_get_ue_golomb(&s->gb);

            if (b->length) {

                b->quant = svq3_get_ue_golomb(&s->gb);

                align_get_bits(&s->gb);

                b->coeff_data = s->gb.buffer + get_bits_count(&s->gb)/8;

                b->length = FFMIN(b->length, get_bits_left(&s->gb)/8);

                skip_bits_long(&s->gb, b->length*8);

            }

        }

        

        if (s->is_arith)

            avctx->execute(avctx, decode_subband_arith, &s->plane[comp].band[level][!!level],

                           NULL, 4-!!level, sizeof(SubBand));

    }

    

    if (!s->is_arith)

        avctx->execute(avctx, decode_subband_golomb, bands, NULL, num_bands, sizeof(SubBand*));

}