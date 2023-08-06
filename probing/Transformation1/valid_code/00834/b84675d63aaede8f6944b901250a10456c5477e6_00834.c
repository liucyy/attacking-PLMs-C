static int ffmmal_read_frame(AVCodecContext *avctx, AVFrame *frame, int *got_frame)

{

    MMALDecodeContext *ctx = avctx->priv_data;

    MMAL_BUFFER_HEADER_T *buffer = NULL;

    MMAL_STATUS_T status = 0;

    int ret = 0;



    if (ctx->eos_received)

        goto done;



    while (1) {

        

        

        

        

        

        

        

        

        

        

        

        if (ctx->frames_output || ctx->packets_sent > MAX_DELAYED_FRAMES || ctx->eos_sent) {

            buffer = mmal_queue_wait(ctx->queue_decoded_frames);

        } else {

            buffer = mmal_queue_get(ctx->queue_decoded_frames);

        }

        if (!buffer)

            goto done;



        ctx->eos_received |= !!(buffer->flags & MMAL_BUFFER_HEADER_FLAG_EOS);

        if (ctx->eos_received)

            goto done;



        if (buffer->cmd == MMAL_EVENT_FORMAT_CHANGED) {

            MMAL_COMPONENT_T *decoder = ctx->decoder;

            MMAL_EVENT_FORMAT_CHANGED_T *ev = mmal_event_format_changed_get(buffer);

            MMAL_BUFFER_HEADER_T *stale_buffer;



            av_log(avctx, AV_LOG_INFO, "Changing output format.\n");



            if ((status = mmal_port_disable(decoder->output[0])))

                goto done;



            while ((stale_buffer = mmal_queue_get(ctx->queue_decoded_frames)))

                mmal_buffer_header_release(stale_buffer);



            mmal_format_copy(decoder->output[0]->format, ev->format);



            if ((ret = ffmal_update_format(avctx)) < 0)

                goto done;



            if ((status = mmal_port_enable(decoder->output[0], output_callback)))

                goto done;



            if ((ret = ffmmal_fill_output_port(avctx)) < 0)

                goto done;



            if ((ret = ffmmal_fill_input_port(avctx)) < 0)

                goto done;



            mmal_buffer_header_release(buffer);

            continue;

        } else if (buffer->cmd) {

            char s[20];

            av_get_codec_tag_string(s, sizeof(s), buffer->cmd);

            av_log(avctx, AV_LOG_WARNING, "Unknown MMAL event %s on output port\n", s);

            goto done;

        } else if (buffer->length == 0) {

            

            mmal_buffer_header_release(buffer);

            continue;

        }



        ctx->frames_output++;



        if ((ret = ffmal_copy_frame(avctx, frame, buffer)) < 0)

            goto done;



        *got_frame = 1;

        break;

    }



done:

    if (buffer)

        mmal_buffer_header_release(buffer);

    if (status && ret >= 0)

        ret = AVERROR_UNKNOWN;

    return ret;

}