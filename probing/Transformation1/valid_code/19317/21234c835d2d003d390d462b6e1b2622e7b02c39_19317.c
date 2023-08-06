static int load_data(AVFilterContext *ctx, int azim, int elev, float radius)

{

    struct SOFAlizerContext *s = ctx->priv;

    const int n_samples = s->sofa.n_samples;

    int n_conv = s->n_conv; 

    int n_fft = s->n_fft;

    int delay_l[16]; 

    int delay_r[16];

    int nb_input_channels = ctx->inputs[0]->channels; 

    float gain_lin = expf((s->gain - 3 * nb_input_channels) / 20 * M_LN10); 

    FFTComplex *data_hrtf_l = NULL;

    FFTComplex *data_hrtf_r = NULL;

    FFTComplex *fft_in_l = NULL;

    FFTComplex *fft_in_r = NULL;

    float *data_ir_l = NULL;

    float *data_ir_r = NULL;

    int offset = 0; 

    int m[16]; 

    int i, j, azim_orig = azim, elev_orig = elev;



    if (!s->sofa.ncid) { 

        av_log(ctx, AV_LOG_ERROR, "Selected SOFA file is invalid. Please select valid SOFA file.\n");

        return AVERROR_INVALIDDATA;

    }



    if (s->type == TIME_DOMAIN) {

        s->temp_src[0] = av_calloc(FFALIGN(n_samples, 16), sizeof(float));

        s->temp_src[1] = av_calloc(FFALIGN(n_samples, 16), sizeof(float));



        

        data_ir_l = av_malloc_array(n_conv * n_samples, sizeof(*data_ir_l));

        data_ir_r = av_malloc_array(n_conv * n_samples, sizeof(*data_ir_r));

        if (!data_ir_r || !data_ir_l || !s->temp_src[0] || !s->temp_src[1]) {

            av_free(data_ir_l);

            av_free(data_ir_r);

            return AVERROR(ENOMEM);

        }

    } else {

        

        data_hrtf_l = av_malloc_array(n_fft, sizeof(*data_hrtf_l) * n_conv);

        data_hrtf_r = av_malloc_array(n_fft, sizeof(*data_hrtf_r) * n_conv);

        if (!data_hrtf_r || !data_hrtf_l) {

            av_free(data_hrtf_l);

            av_free(data_hrtf_r);

            return AVERROR(ENOMEM);

        }

    }



    for (i = 0; i < s->n_conv; i++) {

        

        azim = (int)(s->speaker_azim[i] + azim_orig) % 360;

        elev = (int)(s->speaker_elev[i] + elev_orig) % 90;

        

        m[i] = find_m(s, azim, elev, radius);



        

        delay_l[i] = *(s->sofa.data_delay + 2 * m[i]);

        delay_r[i] = *(s->sofa.data_delay + 2 * m[i] + 1);



        if (s->type == TIME_DOMAIN) {

            offset = i * n_samples; 

            for (j = 0; j < n_samples; j++) {

                

                *(data_ir_l + offset + j) = 

                *(s->sofa.data_ir + 2 * m[i] * n_samples + n_samples - 1 - j) * gain_lin;

                *(data_ir_r + offset + j) = 

                *(s->sofa.data_ir + 2 * m[i] * n_samples + n_samples - 1 - j  + n_samples) * gain_lin;

            }

        } else {

            fft_in_l = av_calloc(n_fft, sizeof(*fft_in_l));

            fft_in_r = av_calloc(n_fft, sizeof(*fft_in_r));

            if (!fft_in_l || !fft_in_r) {

                av_free(data_hrtf_l);

                av_free(data_hrtf_r);

                av_free(fft_in_l);

                av_free(fft_in_r);

                return AVERROR(ENOMEM);

            }



            offset = i * n_fft; 

            for (j = 0; j < n_samples; j++) {

                

                fft_in_l[delay_l[i] + j].re = 

                *(s->sofa.data_ir + 2 * m[i] * n_samples + j) * gain_lin;

                fft_in_r[delay_r[i] + j].re = 

                *(s->sofa.data_ir + (2 * m[i] + 1) * n_samples + j) * gain_lin;

            }



            

            av_fft_permute(s->fft[0], fft_in_l);

            av_fft_calc(s->fft[0], fft_in_l);

            memcpy(data_hrtf_l + offset, fft_in_l, n_fft * sizeof(*fft_in_l));

            av_fft_permute(s->fft[0], fft_in_r);

            av_fft_calc(s->fft[0], fft_in_r);

            memcpy(data_hrtf_r + offset, fft_in_r, n_fft * sizeof(*fft_in_r));

        }



        av_log(ctx, AV_LOG_DEBUG, "Index: %d, Azimuth: %f, Elevation: %f, Radius: %f of SOFA file.\n",

               m[i], *(s->sofa.sp_a + m[i]), *(s->sofa.sp_e + m[i]), *(s->sofa.sp_r + m[i]));

    }



    if (s->type == TIME_DOMAIN) {

        

        memcpy(s->data_ir[0], data_ir_l, sizeof(float) * n_conv * n_samples);

        memcpy(s->data_ir[1], data_ir_r, sizeof(float) * n_conv * n_samples);



        av_freep(&data_ir_l); 

        av_freep(&data_ir_r);

    } else {

        s->data_hrtf[0] = av_malloc_array(n_fft * s->n_conv, sizeof(FFTComplex));

        s->data_hrtf[1] = av_malloc_array(n_fft * s->n_conv, sizeof(FFTComplex));

        if (!s->data_hrtf[0] || !s->data_hrtf[1]) {

            av_freep(&data_hrtf_l);

            av_freep(&data_hrtf_r);

            av_freep(&fft_in_l);

            av_freep(&fft_in_r);

            return AVERROR(ENOMEM); 

        }



        memcpy(s->data_hrtf[0], data_hrtf_l, 

            sizeof(FFTComplex) * n_conv * n_fft); 

        memcpy(s->data_hrtf[1], data_hrtf_r,

            sizeof(FFTComplex) * n_conv * n_fft);



        av_freep(&data_hrtf_l); 

        av_freep(&data_hrtf_r);



        av_freep(&fft_in_l); 

        av_freep(&fft_in_r);

    }



    memcpy(s->delay[0], &delay_l[0], sizeof(int) * s->n_conv);

    memcpy(s->delay[1], &delay_r[0], sizeof(int) * s->n_conv);



    return 0;

}