static int sofalizer_convolute(AVFilterContext *ctx, void *arg, int jobnr, int nb_jobs)

{

    SOFAlizerContext *s = ctx->priv;

    ThreadData *td = arg;

    AVFrame *in = td->in, *out = td->out;

    int offset = jobnr;

    int *write = &td->write[jobnr];

    const int *const delay = td->delay[jobnr];

    const float *const ir = td->ir[jobnr];

    int *n_clippings = &td->n_clippings[jobnr];

    float *ringbuffer = td->ringbuffer[jobnr];

    float *temp_src = td->temp_src[jobnr];

    const int n_samples = s->sofa.n_samples; 

    const float *src = (const float *)in->data[0]; 

    float *dst = (float *)out->data[0]; 

    const int in_channels = s->n_conv; 

    

    const int buffer_length = s->buffer_length;

    

    const uint32_t modulo = (uint32_t)buffer_length - 1;

    float *buffer[16]; 

    int wr = *write;

    int read;

    int i, l;



    dst += offset;

    for (l = 0; l < in_channels; l++) {

        

        buffer[l] = ringbuffer + l * buffer_length;

    }



    for (i = 0; i < in->nb_samples; i++) {

        const float *temp_ir = ir; 



        *dst = 0;

        for (l = 0; l < in_channels; l++) {

            

            *(buffer[l] + wr) = src[l];

        }



        

        for (l = 0; l < in_channels; l++) {

            const float *const bptr = buffer[l];



            if (l == s->lfe_channel) {

                

                

                *dst += *(buffer[s->lfe_channel] + wr) * s->gain_lfe;

                temp_ir += n_samples;

                continue;

            }



            

            read = (wr - *(delay + l) - (n_samples - 1) + buffer_length) & modulo;



            if (read + n_samples < buffer_length) {

                memcpy(temp_src, bptr + read, n_samples * sizeof(*temp_src));

            } else {

                int len = FFMIN(n_samples - (read % n_samples), buffer_length - read);



                memcpy(temp_src, bptr + read, len * sizeof(*temp_src));

                memcpy(temp_src + len, bptr, (n_samples - len) * sizeof(*temp_src));

            }



            

            dst[0] += s->fdsp->scalarproduct_float(temp_ir, temp_src, n_samples);

            temp_ir += n_samples;

        }



        

        if (fabs(*dst) > 1)

            *n_clippings += 1;



        

        dst += 2;

        src += in_channels;

        wr   = (wr + 1) & modulo; 

    }



    *write = wr; 



    return 0;

}