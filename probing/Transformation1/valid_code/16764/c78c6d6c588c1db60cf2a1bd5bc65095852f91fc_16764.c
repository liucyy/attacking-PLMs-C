static int ac3_decode_frame(AVCodecContext * avctx, void *data, int *data_size,

                            const uint8_t *buf, int buf_size)

{

    AC3DecodeContext *s = avctx->priv_data;

    int16_t *out_samples = (int16_t *)data;

    int i, blk, ch, err;



    

    if (s->input_buffer) {

        

        memcpy(s->input_buffer, buf, FFMIN(buf_size, AC3_MAX_FRAME_SIZE));

        init_get_bits(&s->gbc, s->input_buffer, buf_size * 8);

    } else {

        init_get_bits(&s->gbc, buf, buf_size * 8);

    }



    

    err = ac3_parse_header(s);

    if(err) {

        switch(err) {

            case AC3_PARSE_ERROR_SYNC:

                av_log(avctx, AV_LOG_ERROR, "frame sync error : cannot use error concealment\n");

                return -1;

            case AC3_PARSE_ERROR_BSID:

                av_log(avctx, AV_LOG_ERROR, "invalid bitstream id\n");

                break;

            case AC3_PARSE_ERROR_SAMPLE_RATE:

                av_log(avctx, AV_LOG_ERROR, "invalid sample rate\n");

                break;

            case AC3_PARSE_ERROR_FRAME_SIZE:

                av_log(avctx, AV_LOG_ERROR, "invalid frame size\n");

                break;

            case AC3_PARSE_ERROR_FRAME_TYPE:

                av_log(avctx, AV_LOG_ERROR, "invalid frame type\n");

                break;

            default:

                av_log(avctx, AV_LOG_ERROR, "invalid header\n");

                break;

        }

    }



    

    if(s->frame_size > buf_size) {

        av_log(avctx, AV_LOG_ERROR, "incomplete frame\n");

        return -1;

    }



    

    if(!err && avctx->error_resilience >= FF_ER_CAREFUL) {

        if(av_crc(av_crc_get_table(AV_CRC_16_ANSI), 0, &buf[2], s->frame_size-2)) {

            av_log(avctx, AV_LOG_ERROR, "frame CRC mismatch\n");

            err = 1;

        }

    }



    

    if (!err) {

        avctx->sample_rate = s->sample_rate;

        avctx->bit_rate = s->bit_rate;



        

        s->out_channels = s->channels;

        s->output_mode = s->channel_mode;

        if(s->lfe_on)

            s->output_mode |= AC3_OUTPUT_LFEON;

        if (avctx->request_channels > 0 && avctx->request_channels <= 2 &&

                avctx->request_channels < s->channels) {

            s->out_channels = avctx->request_channels;

            s->output_mode  = avctx->request_channels == 1 ? AC3_CHMODE_MONO : AC3_CHMODE_STEREO;

        }

        avctx->channels = s->out_channels;



        

        if(s->channels != s->out_channels && !((s->output_mode & AC3_OUTPUT_LFEON) &&

                s->fbw_channels == s->out_channels)) {

            set_downmix_coeffs(s);

        }

    } else if (!s->out_channels) {

        s->out_channels = avctx->channels;

        if(s->out_channels < s->channels)

            s->output_mode  = s->out_channels == 1 ? AC3_CHMODE_MONO : AC3_CHMODE_STEREO;

    }



    

    for (blk = 0; blk < NB_BLOCKS; blk++) {

        if (!err && ac3_parse_audio_block(s, blk)) {

            av_log(avctx, AV_LOG_ERROR, "error parsing the audio block\n");

        }

        for (i = 0; i < 256; i++)

            for (ch = 0; ch < s->out_channels; ch++)

                *(out_samples++) = s->int_output[ch][i];

    }

    *data_size = NB_BLOCKS * 256 * avctx->channels * sizeof (int16_t);

    return s->frame_size;

}