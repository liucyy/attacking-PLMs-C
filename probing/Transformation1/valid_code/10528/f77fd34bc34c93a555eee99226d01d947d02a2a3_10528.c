static av_cold int aac_decode_init(AVCodecContext *avctx)

{

    AACContext *ac = avctx->priv_data;

    float output_scale_factor;



    ac->avctx = avctx;

    ac->oc[1].m4ac.sample_rate = avctx->sample_rate;



    if (avctx->extradata_size > 0) {

        if (decode_audio_specific_config(ac, ac->avctx, &ac->oc[1].m4ac,

                                         avctx->extradata,

                                         avctx->extradata_size*8, 1) < 0)

            return -1;

    } else {

        int sr, i;

        uint8_t layout_map[MAX_ELEM_ID*4][3];

        int layout_map_tags;



        sr = sample_rate_idx(avctx->sample_rate);

        ac->oc[1].m4ac.sampling_index = sr;

        ac->oc[1].m4ac.channels = avctx->channels;

        ac->oc[1].m4ac.sbr = -1;

        ac->oc[1].m4ac.ps = -1;



        for (i = 0; i < FF_ARRAY_ELEMS(ff_mpeg4audio_channels); i++)

            if (ff_mpeg4audio_channels[i] == avctx->channels)

                break;

        if (i == FF_ARRAY_ELEMS(ff_mpeg4audio_channels)) {

            i = 0;

        }

        ac->oc[1].m4ac.chan_config = i;



        if (ac->oc[1].m4ac.chan_config) {

            int ret = set_default_channel_config(avctx, layout_map,

                &layout_map_tags, ac->oc[1].m4ac.chan_config);

            if (!ret)

                output_configure(ac, layout_map, layout_map_tags,

                                 ac->oc[1].m4ac.chan_config, OC_GLOBAL_HDR);

            else if (avctx->err_recognition & AV_EF_EXPLODE)

                return AVERROR_INVALIDDATA;

        }

    }



    if (avctx->request_sample_fmt == AV_SAMPLE_FMT_FLT) {

        avctx->sample_fmt = AV_SAMPLE_FMT_FLT;

        output_scale_factor = 1.0 / 32768.0;

    } else {

        avctx->sample_fmt = AV_SAMPLE_FMT_S16;

        output_scale_factor = 1.0;

    }



    AAC_INIT_VLC_STATIC( 0, 304);

    AAC_INIT_VLC_STATIC( 1, 270);

    AAC_INIT_VLC_STATIC( 2, 550);

    AAC_INIT_VLC_STATIC( 3, 300);

    AAC_INIT_VLC_STATIC( 4, 328);

    AAC_INIT_VLC_STATIC( 5, 294);

    AAC_INIT_VLC_STATIC( 6, 306);

    AAC_INIT_VLC_STATIC( 7, 268);

    AAC_INIT_VLC_STATIC( 8, 510);

    AAC_INIT_VLC_STATIC( 9, 366);

    AAC_INIT_VLC_STATIC(10, 462);



    ff_aac_sbr_init();



    ff_dsputil_init(&ac->dsp, avctx);

    ff_fmt_convert_init(&ac->fmt_conv, avctx);

    avpriv_float_dsp_init(&ac->fdsp, avctx->flags & CODEC_FLAG_BITEXACT);



    ac->random_state = 0x1f2e3d4c;



    ff_aac_tableinit();



    INIT_VLC_STATIC(&vlc_scalefactors,7,FF_ARRAY_ELEMS(ff_aac_scalefactor_code),

                    ff_aac_scalefactor_bits, sizeof(ff_aac_scalefactor_bits[0]), sizeof(ff_aac_scalefactor_bits[0]),

                    ff_aac_scalefactor_code, sizeof(ff_aac_scalefactor_code[0]), sizeof(ff_aac_scalefactor_code[0]),

                    352);



    ff_mdct_init(&ac->mdct,       11, 1, output_scale_factor/1024.0);

    ff_mdct_init(&ac->mdct_small,  8, 1, output_scale_factor/128.0);

    ff_mdct_init(&ac->mdct_ltp,   11, 0, -2.0/output_scale_factor);

    

    ff_kbd_window_init(ff_aac_kbd_long_1024, 4.0, 1024);

    ff_kbd_window_init(ff_aac_kbd_short_128, 6.0, 128);

    ff_init_ff_sine_windows(10);

    ff_init_ff_sine_windows( 7);



    cbrt_tableinit();



    avcodec_get_frame_defaults(&ac->frame);

    avctx->coded_frame = &ac->frame;



    return 0;

}