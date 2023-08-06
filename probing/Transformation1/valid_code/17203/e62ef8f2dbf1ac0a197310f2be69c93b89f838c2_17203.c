static av_cold void set_bandwidth(AC3EncodeContext *s, int cutoff)

{

    int ch, bw_code;



    if (cutoff) {

        

        int fbw_coeffs;

        cutoff         = av_clip(cutoff, 1, s->sample_rate >> 1);

        fbw_coeffs     = cutoff * 2 * AC3_MAX_COEFS / s->sample_rate;

        bw_code        = av_clip((fbw_coeffs - 73) / 3, 0, 60);

    } else {

        

        

        bw_code = 50;

    }



    

    for (ch = 0; ch < s->fbw_channels; ch++) {

        s->bandwidth_code[ch] = bw_code;

        s->nb_coefs[ch]       = bw_code * 3 + 73;

    }

    if (s->lfe_on)

        s->nb_coefs[s->lfe_channel] = 7; 

}