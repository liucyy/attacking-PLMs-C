static int libschroedinger_encode_frame(AVCodecContext *avctx, AVPacket *pkt,

                                        const AVFrame *frame, int *got_packet)

{

    int enc_size = 0;

    SchroEncoderParams *p_schro_params = avctx->priv_data;

    SchroEncoder *encoder = p_schro_params->encoder;

    struct FFSchroEncodedFrame *p_frame_output = NULL;

    int go = 1;

    SchroBuffer *enc_buf;

    int presentation_frame;

    int parse_code;

    int last_frame_in_sequence = 0;

    int pkt_size, ret;



    if (!frame) {

        

        if (!p_schro_params->eos_signalled) {

            schro_encoder_end_of_stream(encoder);

            p_schro_params->eos_signalled = 1;

        }

    } else {

        

        SchroFrame *in_frame = libschroedinger_frame_from_data(avctx, frame);

        if (!in_frame)

            return AVERROR(ENOMEM);

        

        schro_encoder_push_frame(encoder, in_frame);

    }



    if (p_schro_params->eos_pulled)

        go = 0;



    

    while (go) {

        int err;

        SchroStateEnum state;

        state = schro_encoder_wait(encoder);

        switch (state) {

        case SCHRO_STATE_HAVE_BUFFER:

        case SCHRO_STATE_END_OF_STREAM:

            enc_buf = schro_encoder_pull(encoder, &presentation_frame);

            if (enc_buf->length <= 0)

                return AVERROR_BUG;

            parse_code = enc_buf->data[4];



            

            if ((err = av_reallocp(&p_schro_params->enc_buf,

                                   p_schro_params->enc_buf_size +

                                   enc_buf->length)) < 0) {

                p_schro_params->enc_buf_size = 0;

                return err;

            }



            memcpy(p_schro_params->enc_buf + p_schro_params->enc_buf_size,

                   enc_buf->data, enc_buf->length);

            p_schro_params->enc_buf_size += enc_buf->length;





            if (state == SCHRO_STATE_END_OF_STREAM) {

                p_schro_params->eos_pulled = 1;

                go = 0;

            }



            if (!SCHRO_PARSE_CODE_IS_PICTURE(parse_code)) {

                schro_buffer_unref(enc_buf);

                break;

            }



            

            p_frame_output = av_mallocz(sizeof(FFSchroEncodedFrame));

            if (!p_frame_output)

                return AVERROR(ENOMEM);

            

            p_frame_output->size     = p_schro_params->enc_buf_size;

            p_frame_output->p_encbuf = p_schro_params->enc_buf;

            if (SCHRO_PARSE_CODE_IS_INTRA(parse_code) &&

                SCHRO_PARSE_CODE_IS_REFERENCE(parse_code))

                p_frame_output->key_frame = 1;



            

            p_frame_output->frame_num = AV_RB32(enc_buf->data + 13);



            ff_schro_queue_push_back(&p_schro_params->enc_frame_queue,

                                     p_frame_output);

            p_schro_params->enc_buf_size = 0;

            p_schro_params->enc_buf      = NULL;



            schro_buffer_unref(enc_buf);



            break;



        case SCHRO_STATE_NEED_FRAME:

            go = 0;

            break;



        case SCHRO_STATE_AGAIN:

            break;



        default:

            av_log(avctx, AV_LOG_ERROR, "Unknown Schro Encoder state\n");

            return -1;

        }

    }



    



    if (p_schro_params->enc_frame_queue.size == 1 &&

        p_schro_params->eos_pulled)

        last_frame_in_sequence = 1;



    p_frame_output = ff_schro_queue_pop(&p_schro_params->enc_frame_queue);



    if (!p_frame_output)

        return 0;



    pkt_size = p_frame_output->size;

    if (last_frame_in_sequence && p_schro_params->enc_buf_size > 0)

        pkt_size += p_schro_params->enc_buf_size;

    if ((ret = ff_alloc_packet2(avctx, pkt, pkt_size, 0)) < 0)

        goto error;



    memcpy(pkt->data, p_frame_output->p_encbuf, p_frame_output->size);

#if FF_API_CODED_FRAME

FF_DISABLE_DEPRECATION_WARNINGS

    avctx->coded_frame->key_frame = p_frame_output->key_frame;

    avctx->coded_frame->pts = p_frame_output->frame_num;

FF_ENABLE_DEPRECATION_WARNINGS

#endif

    

    pkt->pts = p_frame_output->frame_num;

    pkt->dts = p_schro_params->dts++;

    enc_size = p_frame_output->size;



    

    if (last_frame_in_sequence && p_schro_params->enc_buf_size > 0) {

        memcpy(pkt->data + enc_size, p_schro_params->enc_buf,

               p_schro_params->enc_buf_size);

        enc_size += p_schro_params->enc_buf_size;

        av_freep(&p_schro_params->enc_buf);

        p_schro_params->enc_buf_size = 0;

    }



    if (p_frame_output->key_frame)

        pkt->flags |= AV_PKT_FLAG_KEY;

    *got_packet = 1;



error:

    

    libschroedinger_free_frame(p_frame_output);

    return ret;

}