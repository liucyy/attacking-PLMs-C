static av_cold int opus_encode_init(AVCodecContext *avctx)

{

    int i, ch, ret;

    OpusEncContext *s = avctx->priv_data;



    s->avctx = avctx;

    s->channels = avctx->channels;



    

    avctx->frame_size = 120;

    

    avctx->initial_padding = 120;



    avctx->cutoff = !avctx->cutoff ? 20000 : avctx->cutoff;



    if (!avctx->bit_rate) {

        int coupled = ff_opus_default_coupled_streams[s->channels - 1];

        avctx->bit_rate = coupled*(96000) + (s->channels - coupled*2)*(48000);

    } else if (avctx->bit_rate < 6000 || avctx->bit_rate > 255000 * s->channels) {

        int64_t clipped_rate = av_clip(avctx->bit_rate, 6000, 255000 * s->channels);

        av_log(avctx, AV_LOG_ERROR, "Unsupported bitrate %"PRId64" kbps, clipping to %"PRId64" kbps\n",

               avctx->bit_rate/1000, clipped_rate/1000);

        avctx->bit_rate = clipped_rate;

    }



    

    s->frame = av_malloc(OPUS_MAX_FRAMES_PER_PACKET*sizeof(CeltFrame));

    if (!s->frame)

        return AVERROR(ENOMEM);

    s->rc = av_malloc(OPUS_MAX_FRAMES_PER_PACKET*sizeof(OpusRangeCoder));

    if (!s->rc)

        return AVERROR(ENOMEM);



    

    avctx->extradata_size = 19;

    avctx->extradata = av_malloc(avctx->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE);

    if (!avctx->extradata)

        return AVERROR(ENOMEM);

    opus_write_extradata(avctx);



    ff_af_queue_init(avctx, &s->afq);



    if (!(s->dsp = avpriv_float_dsp_alloc(avctx->flags & AV_CODEC_FLAG_BITEXACT)))

        return AVERROR(ENOMEM);



    

    for (i = 0; i < CELT_BLOCK_NB; i++)

        if ((ret = ff_mdct15_init(&s->mdct[i], 0, i + 3, 68 << (CELT_BLOCK_NB - 1 - i))))

            return AVERROR(ENOMEM);



    for (i = 0; i < OPUS_MAX_FRAMES_PER_PACKET; i++)

        s->frame[i].block[0].emph_coeff = s->frame[i].block[1].emph_coeff = 0.0f;



    

    for (ch = 0; ch < s->channels; ch++)

        for (i = 0; i < CELT_MAX_BANDS; i++)

            s->last_quantized_energy[ch][i] = 0.0f;



    

    ff_bufqueue_add(avctx, &s->bufqueue, spawn_empty_frame(s));

    if (!ff_bufqueue_peek(&s->bufqueue, 0))

        return AVERROR(ENOMEM);



    return 0;

}