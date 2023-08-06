static void reduce_matrix(AudioMix *am, const double *matrix, int stride)

{

    int i, o;



    memset(am->output_zero, 0, sizeof(am->output_zero));

    memset(am->input_skip,  0, sizeof(am->input_skip));

    memset(am->output_skip, 0, sizeof(am->output_skip));



    

    for (o = 0; o < am->out_channels; o++) {

        int zero = 1;



        

        for (i = 0; i < am->in_channels; i++) {

            if (matrix[o * stride + i] != 0.0) {

                zero = 0;

                break;

            }

        }

        

        if (o < am->in_channels) {

            for (i = 0; i < am->out_channels; i++) {

                if (matrix[i * stride + o] != 0.0) {

                    zero = 0;

                    break;

                }

            }

        }

        if (zero) {

            am->output_zero[o] = 1;

            am->out_matrix_channels--;

        }

    }

    if (am->out_matrix_channels == 0) {

        am->in_matrix_channels = 0;

        return;

    }



    

    for (i = 0; i < FFMIN(am->in_channels, am->out_channels); i++) {

        int skip = 1;



        for (o = 0; o < am->out_channels; o++) {

            int i0;

            if ((o != i && matrix[o * stride + i] != 0.0) ||

                (o == i && matrix[o * stride + i] != 1.0)) {

                skip = 0;

                break;

            }

            

            if (o == i) {

                for (i0 = 0; i0 < am->in_channels; i0++) {

                    if (i0 != i && matrix[o * stride + i0] != 0.0) {

                        skip = 0;

                        break;

                    }

                }

            }

        }

        if (skip) {

            am->input_skip[i] = 1;

            am->in_matrix_channels--;

        }

    }

    

    for (; i < am->in_channels; i++) {

        int contrib = 0;



        for (o = 0; o < am->out_channels; o++) {

            if (matrix[o * stride + i] != 0.0) {

                contrib = 1;

                break;

            }

        }

        if (!contrib) {

            am->input_skip[i] = 1;

            am->in_matrix_channels--;

        }

    }

    if (am->in_matrix_channels == 0) {

        am->out_matrix_channels = 0;

        return;

    }



    

    for (o = 0; o < FFMIN(am->in_channels, am->out_channels); o++) {

        int skip = 1;

        int o0;



        for (i = 0; i < am->in_channels; i++) {

            if ((o != i && matrix[o * stride + i] != 0.0) ||

                (o == i && matrix[o * stride + i] != 1.0)) {

                skip = 0;

                break;

            }

        }

        

        i = o;

        for (o0 = 0; o0 < am->out_channels; o0++) {

            if (o0 != i && matrix[o0 * stride + i] != 0.0) {

                skip = 0;

                break;

            }

        }

        if (skip) {

            am->output_skip[o] = 1;

            am->out_matrix_channels--;

        }

    }

    if (am->out_matrix_channels == 0) {

        am->in_matrix_channels = 0;

        return;

    }

}