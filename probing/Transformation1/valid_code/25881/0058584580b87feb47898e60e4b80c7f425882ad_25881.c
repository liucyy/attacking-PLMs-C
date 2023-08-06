static int _get_transform_coeffs(uint8_t *exps, uint8_t *bap, float chcoeff,

        float *samples, int start, int end, int dith_flag, GetBitContext *gb,

        dither_state *state)

{

    int16_t mantissa;

    int i;

    int gcode;

    mant_group l3_grp, l5_grp, l11_grp;



    for (i = 0; i < 3; i++)

        l3_grp.gcodes[i] = l5_grp.gcodes[i] = l11_grp.gcodes[i] = -1;

    l3_grp.gcptr = l5_grp.gcptr = 3;

    l11_grp.gcptr = 2;



    i = 0;

    while (i < start)

        samples[i++] = 0;



    for (i = start; i < end; i++) {

        switch (bap[i]) {

            case 0:

                if (!dith_flag)

                    mantissa = 0;

                else

                    mantissa = dither_int16(state);

                samples[i] = to_float(exps[i], mantissa) * chcoeff;

                break;



            case 1:

                if (l3_grp.gcptr > 2) {

                    gcode = get_bits(gb, qntztab[1]);

                    if (gcode > 26)

                        return -1;

                    l3_grp.gcodes[0] = gcode / 9;

                    l3_grp.gcodes[1] = (gcode % 9) / 3;

                    l3_grp.gcodes[2] = (gcode % 9) % 3;

                    l3_grp.gcptr = 0;

                }

                mantissa = l3_q_tab[l3_grp.gcodes[l3_grp.gcptr++]];

                samples[i] = to_float(exps[i], mantissa) * chcoeff;

                break;



            case 2:

                if (l5_grp.gcptr > 2) {

                    gcode = get_bits(gb, qntztab[2]);

                    if (gcode > 124)

                        return -1;

                    l5_grp.gcodes[0] = gcode / 25;

                    l5_grp.gcodes[1] = (gcode % 25) / 5;

                    l5_grp.gcodes[2] = (gcode % 25) % 5;

                    l5_grp.gcptr = 0;

                }

                mantissa = l5_q_tab[l5_grp.gcodes[l5_grp.gcptr++]];

                samples[i] = to_float(exps[i], mantissa) * chcoeff;

                break;



            case 3:

                mantissa = get_bits(gb, qntztab[3]);

                if (mantissa > 6)

                    return -1;

                mantissa = l7_q_tab[mantissa];

                samples[i] = to_float(exps[i], mantissa);

                break;



            case 4:

                if (l11_grp.gcptr > 1) {

                    gcode = get_bits(gb, qntztab[4]);

                    if (gcode > 120)

                        return -1;

                    l11_grp.gcodes[0] = gcode / 11;

                    l11_grp.gcodes[1] = gcode % 11;

                }

                mantissa = l11_q_tab[l11_grp.gcodes[l11_grp.gcptr++]];

                samples[i] = to_float(exps[i], mantissa) * chcoeff;

                break;



            case 5:

                mantissa = get_bits(gb, qntztab[5]);

                if (mantissa > 14)

                    return -1;

                mantissa = l15_q_tab[mantissa];

                samples[i] = to_float(exps[i], mantissa) * chcoeff;

                break;



            default:

                mantissa = get_bits(gb, qntztab[bap[i]]) << (16 - qntztab[bap[i]]);

                samples[i] = to_float(exps[i], mantissa) * chcoeff;

                break;

        }

    }



    i = end;

    while (i < 256)

        samples[i++] = 0;



    return 0;

}