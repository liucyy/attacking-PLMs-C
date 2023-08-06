static int decodeTonalComponents (GetBitContext *gb, tonal_component *pComponent, int numBands)

{

    int i,j,k,cnt;

    int   components, coding_mode_selector, coding_mode, coded_values_per_component;

    int   sfIndx, coded_values, max_coded_values, quant_step_index, coded_components;

    int   band_flags[4], mantissa[8];

    float  *pCoef;

    float  scalefactor;

    int   component_count = 0;



    components = get_bits(gb,5);



    

    if (components == 0)

        return 0;



    coding_mode_selector = get_bits(gb,2);

    if (coding_mode_selector == 2)




    coding_mode = coding_mode_selector & 1;



    for (i = 0; i < components; i++) {

        for (cnt = 0; cnt <= numBands; cnt++)

            band_flags[cnt] = get_bits1(gb);



        coded_values_per_component = get_bits(gb,3);



        quant_step_index = get_bits(gb,3);

        if (quant_step_index <= 1)




        if (coding_mode_selector == 3)

            coding_mode = get_bits1(gb);



        for (j = 0; j < (numBands + 1) * 4; j++) {

            if (band_flags[j >> 2] == 0)

                continue;



            coded_components = get_bits(gb,3);



            for (k=0; k<coded_components; k++) {

                sfIndx = get_bits(gb,6);



                pComponent[component_count].pos = j * 64 + (get_bits(gb,6));

                max_coded_values = SAMPLES_PER_FRAME - pComponent[component_count].pos;

                coded_values = coded_values_per_component + 1;

                coded_values = FFMIN(max_coded_values,coded_values);



                scalefactor = ff_atrac_sf_table[sfIndx] * iMaxQuant[quant_step_index];



                readQuantSpectralCoeffs(gb, quant_step_index, coding_mode, mantissa, coded_values);



                pComponent[component_count].numCoefs = coded_values;



                

                pCoef = pComponent[component_count].coef;

                for (cnt = 0; cnt < coded_values; cnt++)

                    pCoef[cnt] = mantissa[cnt] * scalefactor;



                component_count++;

            }

        }

    }



    return component_count;

}