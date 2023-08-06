void ff_g729_postfilter(DSPContext *dsp, int16_t* ht_prev_data, int* voicing,

                     const int16_t *lp_filter_coeffs, int pitch_delay_int,

                     int16_t* residual, int16_t* res_filter_data,

                     int16_t* pos_filter_data, int16_t *speech, int subframe_size)

{

    int16_t residual_filt_buf[SUBFRAME_SIZE+10];

    int16_t lp_gn[33]; 

    int16_t lp_gd[11]; 

    int tilt_comp_coeff;

    int i;



    

    memset(lp_gn, 0, 33 * sizeof(int16_t));



    

    for (i = 0; i < 10; i++)

        lp_gn[i + 11] = (lp_filter_coeffs[i + 1] * formant_pp_factor_num_pow[i] + 0x4000) >> 15;



    

    for (i = 0; i < 10; i++)

        lp_gd[i + 1] = (lp_filter_coeffs[i + 1] * formant_pp_factor_den_pow[i] + 0x4000) >> 15;



    

    memcpy(speech - 10, res_filter_data, 10 * sizeof(int16_t));

    residual_filter(residual + RES_PREV_DATA_SIZE, lp_gn + 11, speech, subframe_size);

    

    memcpy(res_filter_data, speech + subframe_size - 10, 10 * sizeof(int16_t));



    

    *voicing = FFMAX(*voicing, long_term_filter(dsp, pitch_delay_int,

                                                residual, residual_filt_buf + 10,

                                                subframe_size));



    

    memmove(residual, residual + subframe_size, RES_PREV_DATA_SIZE * sizeof(int16_t));



    

    tilt_comp_coeff = get_tilt_comp(dsp, lp_gn, lp_gd, residual_filt_buf + 10, subframe_size);



    

    ff_celp_lp_synthesis_filter(pos_filter_data + 10, lp_gd + 1,

                                residual_filt_buf + 10,

                                subframe_size, 10, 0, 0, 0x800);

    memcpy(pos_filter_data, pos_filter_data + subframe_size, 10 * sizeof(int16_t));



    *ht_prev_data = apply_tilt_comp(speech, pos_filter_data + 10, tilt_comp_coeff,

                                    subframe_size, *ht_prev_data);

}