static int alac_decode_frame(AVCodecContext *avctx,

                             void *outbuffer, int *outputsize,

                             AVPacket *avpkt)

{

    const uint8_t *inbuffer = avpkt->data;

    int input_buffer_size = avpkt->size;

    ALACContext *alac = avctx->priv_data;



    int channels;

    unsigned int outputsamples;

    int hassize;

    unsigned int readsamplesize;

    int isnotcompressed;

    uint8_t interlacing_shift;

    uint8_t interlacing_leftweight;

    int i, ch;



    

    if (!inbuffer || !input_buffer_size)

        return -1;



    init_get_bits(&alac->gb, inbuffer, input_buffer_size * 8);



    channels = get_bits(&alac->gb, 3) + 1;

    if (channels != avctx->channels) {

        av_log(avctx, AV_LOG_ERROR, "frame header channel count mismatch\n");

        return AVERROR_INVALIDDATA;

    }



    

    skip_bits(&alac->gb, 4);



    skip_bits(&alac->gb, 12); 



    

    hassize = get_bits1(&alac->gb);



    alac->extra_bits = get_bits(&alac->gb, 2) << 3;



    

    isnotcompressed = get_bits1(&alac->gb);



    if (hassize) {

        

        outputsamples = get_bits_long(&alac->gb, 32);

        if(outputsamples > alac->setinfo_max_samples_per_frame){

            av_log(avctx, AV_LOG_ERROR, "outputsamples %d > %d\n", outputsamples, alac->setinfo_max_samples_per_frame);

            return -1;

        }

    } else

        outputsamples = alac->setinfo_max_samples_per_frame;



    alac->bytespersample = channels * av_get_bytes_per_sample(avctx->sample_fmt);



    if(outputsamples > *outputsize / alac->bytespersample){

        av_log(avctx, AV_LOG_ERROR, "sample buffer too small\n");

        return -1;

    }



    *outputsize = outputsamples * alac->bytespersample;

    readsamplesize = alac->setinfo_sample_size - alac->extra_bits + channels - 1;

    if (readsamplesize > MIN_CACHE_BITS) {

        av_log(avctx, AV_LOG_ERROR, "readsamplesize too big (%d)\n", readsamplesize);

        return -1;

    }



    if (!isnotcompressed) {

        

        int16_t predictor_coef_table[MAX_CHANNELS][32];

        int predictor_coef_num[MAX_CHANNELS];

        int prediction_type[MAX_CHANNELS];

        int prediction_quantitization[MAX_CHANNELS];

        int ricemodifier[MAX_CHANNELS];



        interlacing_shift = get_bits(&alac->gb, 8);

        interlacing_leftweight = get_bits(&alac->gb, 8);



        for (ch = 0; ch < channels; ch++) {

            prediction_type[ch] = get_bits(&alac->gb, 4);

            prediction_quantitization[ch] = get_bits(&alac->gb, 4);



            ricemodifier[ch] = get_bits(&alac->gb, 3);

            predictor_coef_num[ch] = get_bits(&alac->gb, 5);



            

            for (i = 0; i < predictor_coef_num[ch]; i++)

                predictor_coef_table[ch][i] = (int16_t)get_bits(&alac->gb, 16);

        }



        if (alac->extra_bits) {

            for (i = 0; i < outputsamples; i++) {

                for (ch = 0; ch < channels; ch++)

                    alac->extra_bits_buffer[ch][i] = get_bits(&alac->gb, alac->extra_bits);

            }

        }

        for (ch = 0; ch < channels; ch++) {

            bastardized_rice_decompress(alac,

                                        alac->predicterror_buffer[ch],

                                        outputsamples,

                                        readsamplesize,

                                        alac->setinfo_rice_initialhistory,

                                        alac->setinfo_rice_kmodifier,

                                        ricemodifier[ch] * alac->setinfo_rice_historymult / 4,

                                        (1 << alac->setinfo_rice_kmodifier) - 1);



            if (prediction_type[ch] == 0) {

                

                predictor_decompress_fir_adapt(alac->predicterror_buffer[ch],

                                               alac->outputsamples_buffer[ch],

                                               outputsamples,

                                               readsamplesize,

                                               predictor_coef_table[ch],

                                               predictor_coef_num[ch],

                                               prediction_quantitization[ch]);

            } else {

                av_log(avctx, AV_LOG_ERROR, "FIXME: unhandled prediction type: %i\n", prediction_type[ch]);

                

            }

        }

    } else {

        

        for (i = 0; i < outputsamples; i++) {

            for (ch = 0; ch < channels; ch++) {

                alac->outputsamples_buffer[ch][i] = get_sbits_long(&alac->gb,

                                                                   alac->setinfo_sample_size);

            }

        }

        alac->extra_bits = 0;

        interlacing_shift = 0;

        interlacing_leftweight = 0;

    }

    if (get_bits(&alac->gb, 3) != 7)

        av_log(avctx, AV_LOG_ERROR, "Error : Wrong End Of Frame\n");



    if (channels == 2 && interlacing_leftweight) {

        decorrelate_stereo(alac->outputsamples_buffer, outputsamples,

                           interlacing_shift, interlacing_leftweight);

    }



    if (alac->extra_bits) {

        append_extra_bits(alac->outputsamples_buffer, alac->extra_bits_buffer,

                          alac->extra_bits, alac->numchannels, outputsamples);

    }



    switch(alac->setinfo_sample_size) {

    case 16:

        if (channels == 2) {

            interleave_stereo_16(alac->outputsamples_buffer, outbuffer,

                                 outputsamples);

        } else {

            for (i = 0; i < outputsamples; i++) {

                ((int16_t*)outbuffer)[i] = alac->outputsamples_buffer[0][i];

            }

        }

        break;

    case 24:

        if (channels == 2) {

            interleave_stereo_24(alac->outputsamples_buffer, outbuffer,

                                 outputsamples);

        } else {

            for (i = 0; i < outputsamples; i++)

                ((int32_t *)outbuffer)[i] = alac->outputsamples_buffer[0][i] << 8;

        }

        break;

    }



    if (input_buffer_size * 8 - get_bits_count(&alac->gb) > 8)

        av_log(avctx, AV_LOG_ERROR, "Error : %d bits left\n", input_buffer_size * 8 - get_bits_count(&alac->gb));



    return input_buffer_size;

}