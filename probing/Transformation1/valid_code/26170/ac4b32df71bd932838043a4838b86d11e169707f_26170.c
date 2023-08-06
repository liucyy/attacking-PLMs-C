static int decode_block_coeffs_internal(VP56RangeCoder *r, int16_t block[16],

                                        uint8_t probs[16][3][NUM_DCT_TOKENS - 1],

                                        int i, uint8_t *token_prob,

                                        int16_t qmul[2])

{

    VP56RangeCoder c = *r;

    goto skip_eob;

    do {

        int coeff;

        if (!vp56_rac_get_prob_branchy(&c, token_prob[0]))   

            break;



skip_eob:

        if (!vp56_rac_get_prob_branchy(&c, token_prob[1])) { 

            if (++i == 16)

                break; 

            token_prob = probs[i][0];

            goto skip_eob;

        }



        if (!vp56_rac_get_prob_branchy(&c, token_prob[2])) { 

            coeff = 1;

            token_prob = probs[i + 1][1];

        } else {

            if (!vp56_rac_get_prob_branchy(&c, token_prob[3])) { 

                coeff = vp56_rac_get_prob_branchy(&c, token_prob[4]);

                if (coeff)

                    coeff += vp56_rac_get_prob(&c, token_prob[5]);

                coeff += 2;

            } else {

                

                if (!vp56_rac_get_prob_branchy(&c, token_prob[6])) {

                    if (!vp56_rac_get_prob_branchy(&c, token_prob[7])) { 

                        coeff = 5 + vp56_rac_get_prob(&c, vp8_dct_cat1_prob[0]);

                    } else {                                    

                        coeff  = 7;

                        coeff += vp56_rac_get_prob(&c, vp8_dct_cat2_prob[0]) << 1;

                        coeff += vp56_rac_get_prob(&c, vp8_dct_cat2_prob[1]);

                    }

                } else {    

                    int a   = vp56_rac_get_prob(&c, token_prob[8]);

                    int b   = vp56_rac_get_prob(&c, token_prob[9 + a]);

                    int cat = (a << 1) + b;

                    coeff  = 3 + (8 << cat);

                    coeff += vp8_rac_get_coeff(&c, ff_vp8_dct_cat_prob[cat]);

                }

            }

            token_prob = probs[i + 1][2];

        }

        block[zigzag_scan[i]] = (vp8_rac_get(&c) ? -coeff : coeff) * qmul[!!i];

    } while (++i < 16);



    *r = c;

    return i;

}