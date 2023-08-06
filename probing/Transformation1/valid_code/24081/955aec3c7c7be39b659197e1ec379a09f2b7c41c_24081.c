static int decode_frame(AVCodecContext * avctx, void *data, int *got_frame_ptr,

                        AVPacket *avpkt)

{

    const uint8_t *buf  = avpkt->data;

    int buf_size        = avpkt->size;

    MPADecodeContext *s = avctx->priv_data;

    uint32_t header;

    int ret;



    if (buf_size < HEADER_SIZE)

        return AVERROR_INVALIDDATA;



    header = AV_RB32(buf);

    if (ff_mpa_check_header(header) < 0) {

        av_log(avctx, AV_LOG_ERROR, "Header missing\n");

        return AVERROR_INVALIDDATA;

    }



    if (avpriv_mpegaudio_decode_header((MPADecodeHeader *)s, header) == 1) {

        

        s->frame_size = -1;

        return AVERROR_INVALIDDATA;

    }

    

    avctx->channels       = s->nb_channels;

    avctx->channel_layout = s->nb_channels == 1 ? AV_CH_LAYOUT_MONO : AV_CH_LAYOUT_STEREO;

    if (!avctx->bit_rate)

        avctx->bit_rate = s->bit_rate;



    s->frame = data;



    ret = mp_decode_frame(s, NULL, buf, buf_size);

    if (ret >= 0) {

        s->frame->nb_samples = avctx->frame_size;

        *got_frame_ptr       = 1;

        avctx->sample_rate   = s->sample_rate;

        

    } else {

        av_log(avctx, AV_LOG_ERROR, "Error while decoding MPEG audio frame.\n");

        

        *got_frame_ptr = 0;

        if (buf_size == avpkt->size || ret != AVERROR_INVALIDDATA)

            return ret;

    }

    s->frame_size = 0;

    return buf_size;

}