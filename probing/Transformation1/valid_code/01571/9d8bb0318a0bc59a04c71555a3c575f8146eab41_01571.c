static int unpack_dct_coeffs(Vp3DecodeContext *s, GetBitContext *gb)

{

    int i;

    int dc_y_table;

    int dc_c_table;

    int ac_y_table;

    int ac_c_table;

    int residual_eob_run = 0;



    

    dc_y_table = get_bits(gb, 4);

    dc_c_table = get_bits(gb, 4);



    

    residual_eob_run = unpack_vlcs(s, gb, &s->dc_vlc[dc_y_table], 0,

        1, residual_eob_run);



    

    reverse_dc_prediction(s, 0, s->fragment_width, s->fragment_height);



    

    residual_eob_run = unpack_vlcs(s, gb, &s->dc_vlc[dc_c_table], 0,

        0, residual_eob_run);



    

    if (!(s->avctx->flags & CODEC_FLAG_GRAY))

    {

        reverse_dc_prediction(s, s->fragment_start[1],

            s->fragment_width / 2, s->fragment_height / 2);

        reverse_dc_prediction(s, s->fragment_start[2],

            s->fragment_width / 2, s->fragment_height / 2);

    }



    

    ac_y_table = get_bits(gb, 4);

    ac_c_table = get_bits(gb, 4);



    

    for (i = 1; i <= 5; i++) {

        residual_eob_run = unpack_vlcs(s, gb, &s->ac_vlc_1[ac_y_table], i,

            1, residual_eob_run);



        residual_eob_run = unpack_vlcs(s, gb, &s->ac_vlc_1[ac_c_table], i,

            0, residual_eob_run);

    }



    

    for (i = 6; i <= 14; i++) {

        residual_eob_run = unpack_vlcs(s, gb, &s->ac_vlc_2[ac_y_table], i,

            1, residual_eob_run);



        residual_eob_run = unpack_vlcs(s, gb, &s->ac_vlc_2[ac_c_table], i,

            0, residual_eob_run);

    }



    

    for (i = 15; i <= 27; i++) {

        residual_eob_run = unpack_vlcs(s, gb, &s->ac_vlc_3[ac_y_table], i,

            1, residual_eob_run);



        residual_eob_run = unpack_vlcs(s, gb, &s->ac_vlc_3[ac_c_table], i,

            0, residual_eob_run);

    }



    

    for (i = 28; i <= 63; i++) {

        residual_eob_run = unpack_vlcs(s, gb, &s->ac_vlc_4[ac_y_table], i,

            1, residual_eob_run);



        residual_eob_run = unpack_vlcs(s, gb, &s->ac_vlc_4[ac_c_table], i,

            0, residual_eob_run);

    }



    return 0;

}