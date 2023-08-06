static int hls_read_header(AVFormatContext *s)

{

    void *u = (s->flags & AVFMT_FLAG_CUSTOM_IO) ? NULL : s->pb;

    HLSContext *c = s->priv_data;

    int ret = 0, i;

    int highest_cur_seq_no = 0;



    c->ctx                = s;

    c->interrupt_callback = &s->interrupt_callback;

    c->strict_std_compliance = s->strict_std_compliance;



    c->first_packet = 1;

    c->first_timestamp = AV_NOPTS_VALUE;

    c->cur_timestamp = AV_NOPTS_VALUE;



    if (u) {

        

        update_options(&c->user_agent, "user-agent", u);



        

        update_options(&c->cookies, "cookies", u);



        

        update_options(&c->headers, "headers", u);



        

        update_options(&c->http_proxy, "http_proxy", u);

    }



    if ((ret = parse_playlist(c, s->filename, NULL, s->pb)) < 0)

        goto fail;



    if ((ret = save_avio_options(s)) < 0)

        goto fail;



    

    av_dict_set(&c->avio_opts, "seekable", "0", 0);



    if (c->n_variants == 0) {

        av_log(NULL, AV_LOG_WARNING, "Empty playlist\n");

        ret = AVERROR_EOF;

        goto fail;

    }

    

    if (c->n_playlists > 1 || c->playlists[0]->n_segments == 0) {

        for (i = 0; i < c->n_playlists; i++) {

            struct playlist *pls = c->playlists[i];

            if ((ret = parse_playlist(c, pls->url, pls, NULL)) < 0)

                goto fail;

        }

    }



    if (c->variants[0]->playlists[0]->n_segments == 0) {

        av_log(NULL, AV_LOG_WARNING, "Empty playlist\n");

        ret = AVERROR_EOF;

        goto fail;

    }



    

    if (c->variants[0]->playlists[0]->finished) {

        int64_t duration = 0;

        for (i = 0; i < c->variants[0]->playlists[0]->n_segments; i++)

            duration += c->variants[0]->playlists[0]->segments[i]->duration;

        s->duration = duration;

    }



    

    for (i = 0; i < c->n_variants; i++) {

        struct variant *var = c->variants[i];



        if (var->audio_group[0])

            add_renditions_to_variant(c, var, AVMEDIA_TYPE_AUDIO, var->audio_group);

        if (var->video_group[0])

            add_renditions_to_variant(c, var, AVMEDIA_TYPE_VIDEO, var->video_group);

        if (var->subtitles_group[0])

            add_renditions_to_variant(c, var, AVMEDIA_TYPE_SUBTITLE, var->subtitles_group);

    }



    

    for (i = 0; i < c->n_variants; i++) {

        struct variant *v = c->variants[i];

        AVProgram *program;



        program = av_new_program(s, i);

        if (!program)

            goto fail;

        av_dict_set_int(&program->metadata, "variant_bitrate", v->bandwidth, 0);

    }



    

    for (i = 0; i < c->n_playlists; i++) {

        struct playlist *pls = c->playlists[i];



        if (pls->n_segments == 0)

            continue;



        pls->cur_seq_no = select_cur_seq_no(c, pls);

        highest_cur_seq_no = FFMAX(highest_cur_seq_no, pls->cur_seq_no);

    }



    

    for (i = 0; i < c->n_playlists; i++) {

        struct playlist *pls = c->playlists[i];

        AVInputFormat *in_fmt = NULL;



        if (!(pls->ctx = avformat_alloc_context())) {

            ret = AVERROR(ENOMEM);

            goto fail;

        }



        if (pls->n_segments == 0)

            continue;



        pls->index  = i;

        pls->needed = 1;

        pls->parent = s;



        

        if (!pls->finished && pls->cur_seq_no == highest_cur_seq_no - 1 &&

            highest_cur_seq_no < pls->start_seq_no + pls->n_segments) {

            pls->cur_seq_no = highest_cur_seq_no;

        }



        pls->read_buffer = av_malloc(INITIAL_BUFFER_SIZE);

        if (!pls->read_buffer){

            ret = AVERROR(ENOMEM);

            avformat_free_context(pls->ctx);

            pls->ctx = NULL;

            goto fail;

        }

        ffio_init_context(&pls->pb, pls->read_buffer, INITIAL_BUFFER_SIZE, 0, pls,

                          read_data, NULL, NULL);

        pls->pb.seekable = 0;

        ret = av_probe_input_buffer(&pls->pb, &in_fmt, pls->segments[0]->url,

                                    NULL, 0, 0);

        if (ret < 0) {

            

            av_log(s, AV_LOG_ERROR, "Error when loading first segment '%s'\n", pls->segments[0]->url);

            avformat_free_context(pls->ctx);

            pls->ctx = NULL;

            goto fail;

        }

        pls->ctx->pb       = &pls->pb;

        pls->ctx->io_open  = nested_io_open;



        if ((ret = ff_copy_whiteblacklists(pls->ctx, s)) < 0)

            goto fail;



        ret = avformat_open_input(&pls->ctx, pls->segments[0]->url, in_fmt, NULL);

        if (ret < 0)

            goto fail;



        if (pls->id3_deferred_extra && pls->ctx->nb_streams == 1) {

            ff_id3v2_parse_apic(pls->ctx, &pls->id3_deferred_extra);

            avformat_queue_attached_pictures(pls->ctx);

            ff_id3v2_free_extra_meta(&pls->id3_deferred_extra);

            pls->id3_deferred_extra = NULL;

        }



        if (pls->is_id3_timestamped == -1)

            av_log(s, AV_LOG_WARNING, "No expected HTTP requests have been made\n");



        

        if (pls->is_id3_timestamped) {

            ret = avformat_find_stream_info(pls->ctx, NULL);

            if (ret < 0)

                goto fail;

        }



        pls->has_noheader_flag = !!(pls->ctx->ctx_flags & AVFMTCTX_NOHEADER);



        

        ret = update_streams_from_subdemuxer(s, pls);

        if (ret < 0)

            goto fail;



        add_metadata_from_renditions(s, pls, AVMEDIA_TYPE_AUDIO);

        add_metadata_from_renditions(s, pls, AVMEDIA_TYPE_VIDEO);

        add_metadata_from_renditions(s, pls, AVMEDIA_TYPE_SUBTITLE);

    }



    update_noheader_flag(s);



    return 0;

fail:

    free_playlist_list(c);

    free_variant_list(c);

    free_rendition_list(c);

    return ret;

}