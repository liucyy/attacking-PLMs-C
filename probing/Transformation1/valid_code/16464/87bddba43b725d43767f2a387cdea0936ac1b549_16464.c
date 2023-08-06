float ff_amr_set_fixed_gain(float fixed_gain_factor, float fixed_mean_energy,

                            float *prediction_error, float energy_mean,

                            const float *pred_table)

{

    

    

    

    float val = fixed_gain_factor *

        ff_exp10(0.05 *

              (avpriv_scalarproduct_float_c(pred_table, prediction_error, 4) +

               energy_mean)) /

        sqrtf(fixed_mean_energy);



    

    memmove(&prediction_error[0], &prediction_error[1],

            3 * sizeof(prediction_error[0]));

    prediction_error[3] = 20.0 * log10f(fixed_gain_factor);



    return val;

}