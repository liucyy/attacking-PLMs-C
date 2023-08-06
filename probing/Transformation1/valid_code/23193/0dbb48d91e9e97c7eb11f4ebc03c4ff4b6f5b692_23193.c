static int mpeg_mux_init(AVFormatContext *ctx)

{

    MpegMuxContext *s = ctx->priv_data;

    int bitrate, i, mpa_id, mpv_id, ac3_id;

    AVStream *st;

    StreamInfo *stream;



    s->packet_number = 0;

    s->is_vcd = (ctx->oformat == &mpeg1vcd_mux);

    s->is_mpeg2 = (ctx->oformat == &mpeg2vob_mux);

    

    if (s->is_vcd)

        s->packet_size = 2324; 

    else

        s->packet_size = 2048;

        

    

    s->packet_data_max_size = s->packet_size - 7;

    if (s->is_mpeg2)

        s->packet_data_max_size -= 2;

    s->audio_bound = 0;

    s->video_bound = 0;

    mpa_id = AUDIO_ID;

    ac3_id = 0x80;

    mpv_id = VIDEO_ID;

    s->scr_stream_index = -1;

    for(i=0;i<ctx->nb_streams;i++) {

        st = ctx->streams[i];

        stream = av_mallocz(sizeof(StreamInfo));

        if (!stream)

            goto fail;

        st->priv_data = stream;



        switch(st->codec.codec_type) {

        case CODEC_TYPE_AUDIO:

            if (st->codec.codec_id == CODEC_ID_AC3)

                stream->id = ac3_id++;

            else

                stream->id = mpa_id++;

            stream->max_buffer_size = 4 * 1024; 

            s->audio_bound++;

            break;

        case CODEC_TYPE_VIDEO:

            

            if (s->scr_stream_index == -1)

                s->scr_stream_index = i;

            stream->id = mpv_id++;

            stream->max_buffer_size = 46 * 1024; 

            s->video_bound++;

            break;

        default:

            av_abort();

        }

    }

    

    if (s->scr_stream_index == -1)

        s->scr_stream_index = 0;



    

    bitrate = 2000;

    for(i=0;i<ctx->nb_streams;i++) {

        st = ctx->streams[i];

        bitrate += st->codec.bit_rate;

    }

    s->mux_rate = (bitrate + (8 * 50) - 1) / (8 * 50);

    

    if (s->is_vcd || s->is_mpeg2)

        

        s->pack_header_freq = 1;

    else

        

        s->pack_header_freq = 2 * bitrate / s->packet_size / 8;



    

    if (s->pack_header_freq == 0)

       s->pack_header_freq = 1;

    

    if (s->is_mpeg2)

        

        s->system_header_freq = s->pack_header_freq * 40;

    else if (s->is_vcd)

        

        s->system_header_freq = s->pack_header_freq * 40;

    else

        s->system_header_freq = s->pack_header_freq * 5;

    

    for(i=0;i<ctx->nb_streams;i++) {

        stream = ctx->streams[i]->priv_data;

        stream->buffer_ptr = 0;

        stream->packet_number = 0;

        stream->start_pts = AV_NOPTS_VALUE;

        stream->start_dts = AV_NOPTS_VALUE;

    }

    s->last_scr = 0;

    return 0;

 fail:

    for(i=0;i<ctx->nb_streams;i++) {

        av_free(ctx->streams[i]->priv_data);

    }

    return -ENOMEM;

}