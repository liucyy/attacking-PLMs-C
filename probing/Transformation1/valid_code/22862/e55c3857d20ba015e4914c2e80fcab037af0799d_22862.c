static int audio_get_buffer(AVCodecContext *avctx, AVFrame *frame)

{

    AVCodecInternal *avci = avctx->internal;

    InternalBuffer *buf;

    int buf_size, ret;



    buf_size = av_samples_get_buffer_size(NULL, avctx->channels,

                                          frame->nb_samples, avctx->sample_fmt,

                                          0);

    if (buf_size < 0)

        return AVERROR(EINVAL);



    

    if (!avci->buffer) {

        avci->buffer = av_mallocz(sizeof(InternalBuffer));

        if (!avci->buffer)

            return AVERROR(ENOMEM);

    }

    buf = avci->buffer;



    

    if (buf->extended_data) {

        

        if (buf->extended_data[0] && buf_size > buf->audio_data_size) {

            av_free(buf->extended_data[0]);

            if (buf->extended_data != buf->data)

                av_free(buf->extended_data);

            buf->extended_data = NULL;

            buf->data[0]       = NULL;

        }

        

        if (buf->nb_channels != avctx->channels) {

            if (buf->extended_data != buf->data)

                av_free(buf->extended_data);

            buf->extended_data = NULL;

        }

    }



    

    if (!buf->extended_data) {

        if (!buf->data[0]) {

            if (!(buf->data[0] = av_mallocz(buf_size)))

                return AVERROR(ENOMEM);

            buf->audio_data_size = buf_size;

        }

        if ((ret = avcodec_fill_audio_frame(frame, avctx->channels,

                                            avctx->sample_fmt, buf->data[0],

                                            buf->audio_data_size, 0)))

            return ret;



        if (frame->extended_data == frame->data)

            buf->extended_data = buf->data;

        else

            buf->extended_data = frame->extended_data;

        memcpy(buf->data, frame->data, sizeof(frame->data));

        buf->linesize[0] = frame->linesize[0];

        buf->nb_channels = avctx->channels;

    } else {

        

        frame->extended_data = buf->extended_data;

        frame->linesize[0]   = buf->linesize[0];

        memcpy(frame->data, buf->data, sizeof(frame->data));

    }



    frame->type = FF_BUFFER_TYPE_INTERNAL;

    ff_init_buffer_info(avctx, frame);



    if (avctx->debug & FF_DEBUG_BUFFERS)

        av_log(avctx, AV_LOG_DEBUG, "default_get_buffer called on frame %p, "

                                    "internal audio buffer used\n", frame);



    return 0;

}