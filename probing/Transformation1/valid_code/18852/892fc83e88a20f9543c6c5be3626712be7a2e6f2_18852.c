static void unpack_dct_coeffs(Vp3DecodeContext *s, GetBitContext *gb)

{

    int i;

    int dc_y_table;

    int dc_c_table;

    int ac_y_table;

    int ac_c_table;

    int residual_eob_run = 0;



    

    dc_y_table = get_bits(gb, 4);

    dc_c_table = get_bits(gb, 4);



    

    debug_vp3("  vp3: unpacking Y plane DC coefficients using table %d\n",

        dc_y_table);

    residual_eob_run = unpack_vlcs(s, gb, &s->dc_vlc[dc_y_table], 0, 

        s->first_coded_y_fragment, s->last_coded_y_fragment, residual_eob_run);



    

    debug_vp3("  vp3: unpacking C plane DC coefficients using table %d\n",

        dc_c_table);

    residual_eob_run = unpack_vlcs(s, gb, &s->dc_vlc[dc_c_table], 0,

        s->first_coded_c_fragment, s->last_coded_c_fragment, residual_eob_run);



    

    ac_y_table = get_bits(gb, 4);

    ac_c_table = get_bits(gb, 4);



    

    for (i = 1; i <= 5; i++) {



        debug_vp3("  vp3: unpacking level %d Y plane AC coefficients using table %d\n",

            i, ac_y_table);

        residual_eob_run = unpack_vlcs(s, gb, &s->ac_vlc_1[ac_y_table], i, 

            s->first_coded_y_fragment, s->last_coded_y_fragment, residual_eob_run);



        debug_vp3("  vp3: unpacking level %d C plane AC coefficients using table %d\n",

            i, ac_c_table);

        residual_eob_run = unpack_vlcs(s, gb, &s->ac_vlc_1[ac_c_table], i, 

            s->first_coded_c_fragment, s->last_coded_c_fragment, residual_eob_run);

    }



    

    for (i = 6; i <= 14; i++) {



        debug_vp3("  vp3: unpacking level %d Y plane AC coefficients using table %d\n",

            i, ac_y_table);

        residual_eob_run = unpack_vlcs(s, gb, &s->ac_vlc_2[ac_y_table], i, 

            s->first_coded_y_fragment, s->last_coded_y_fragment, residual_eob_run);



        debug_vp3("  vp3: unpacking level %d C plane AC coefficients using table %d\n",

            i, ac_c_table);

        residual_eob_run = unpack_vlcs(s, gb, &s->ac_vlc_2[ac_c_table], i, 

            s->first_coded_c_fragment, s->last_coded_c_fragment, residual_eob_run);

    }



    

    for (i = 15; i <= 27; i++) {



        debug_vp3("  vp3: unpacking level %d Y plane AC coefficients using table %d\n",

            i, ac_y_table);

        residual_eob_run = unpack_vlcs(s, gb, &s->ac_vlc_3[ac_y_table], i, 

            s->first_coded_y_fragment, s->last_coded_y_fragment, residual_eob_run);



        debug_vp3("  vp3: unpacking level %d C plane AC coefficients using table %d\n",

            i, ac_c_table);

        residual_eob_run = unpack_vlcs(s, gb, &s->ac_vlc_3[ac_c_table], i, 

            s->first_coded_c_fragment, s->last_coded_c_fragment, residual_eob_run);

    }



    

    for (i = 28; i <= 63; i++) {



        debug_vp3("  vp3: unpacking level %d Y plane AC coefficients using table %d\n",

            i, ac_y_table);

        residual_eob_run = unpack_vlcs(s, gb, &s->ac_vlc_4[ac_y_table], i, 

            s->first_coded_y_fragment, s->last_coded_y_fragment, residual_eob_run);



        debug_vp3("  vp3: unpacking level %d C plane AC coefficients using table %d\n",

            i, ac_c_table);

        residual_eob_run = unpack_vlcs(s, gb, &s->ac_vlc_4[ac_c_table], i, 

            s->first_coded_c_fragment, s->last_coded_c_fragment, residual_eob_run);

    }

}