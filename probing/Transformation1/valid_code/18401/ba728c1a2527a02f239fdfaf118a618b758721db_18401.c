static int init_resampler(AVCodecContext *input_codec_context,

                          AVCodecContext *output_codec_context,

                          SwrContext **resample_context)

{

    

    if (input_codec_context->sample_fmt != output_codec_context->sample_fmt ||

        input_codec_context->channels != output_codec_context->channels) {

        int error;



        

        *resample_context = swr_alloc_set_opts(NULL,

                                              av_get_default_channel_layout(output_codec_context->channels),

                                              output_codec_context->sample_fmt,

                                              output_codec_context->sample_rate,

                                              av_get_default_channel_layout(input_codec_context->channels),

                                              input_codec_context->sample_fmt,

                                              input_codec_context->sample_rate,

                                              0, NULL);

        if (!*resample_context) {

            fprintf(stderr, "Could not allocate resample context\n");

            return AVERROR(ENOMEM);

        }

        

        av_assert0(output_codec_context->sample_rate == input_codec_context->sample_rate);



        

        if ((error = swr_init(*resample_context)) < 0) {

            fprintf(stderr, "Could not open resample context\n");

            swr_free(resample_context);

            return error;

        }

    }

    return 0;

}