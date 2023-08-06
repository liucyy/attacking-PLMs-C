static av_cold int decode_init_mp3on4(AVCodecContext * avctx)

{

    MP3On4DecodeContext *s = avctx->priv_data;

    MPEG4AudioConfig cfg;

    int i;



    if ((avctx->extradata_size < 2) || (avctx->extradata == NULL)) {

        av_log(avctx, AV_LOG_ERROR, "Codec extradata missing or too short.\n");

        return AVERROR_INVALIDDATA;

    }



    avpriv_mpeg4audio_get_config(&cfg, avctx->extradata,

                                 avctx->extradata_size * 8, 1);

    if (!cfg.chan_config || cfg.chan_config > 7) {

        av_log(avctx, AV_LOG_ERROR, "Invalid channel config number.\n");

        return AVERROR_INVALIDDATA;

    }

    s->frames             = mp3Frames[cfg.chan_config];

    s->coff               = chan_offset[cfg.chan_config];

    avctx->channels       = ff_mpeg4audio_channels[cfg.chan_config];

    avctx->channel_layout = chan_layout[cfg.chan_config];



    if (cfg.sample_rate < 16000)

        s->syncword = 0xffe00000;

    else

        s->syncword = 0xfff00000;



    

    

    s->mp3decctx[0] = av_mallocz(sizeof(MPADecodeContext));

    if (!s->mp3decctx[0])

        goto alloc_fail;

    

    avctx->priv_data = s->mp3decctx[0];

    decode_init(avctx);

    

    avctx->priv_data = s;

    s->mp3decctx[0]->adu_mode = 1; 



    

    for (i = 1; i < s->frames; i++) {

        s->mp3decctx[i] = av_mallocz(sizeof(MPADecodeContext));

        if (!s->mp3decctx[i])

            goto alloc_fail;

        s->mp3decctx[i]->adu_mode = 1;

        s->mp3decctx[i]->avctx = avctx;

        s->mp3decctx[i]->mpadsp = s->mp3decctx[0]->mpadsp;

    }



    return 0;

alloc_fail:

    decode_close_mp3on4(avctx);

    return AVERROR(ENOMEM);

}