AVFormatContext *ff_rtp_chain_mux_open(AVFormatContext *s, AVStream *st,

                                       URLContext *handle, int packet_size)

{

    AVFormatContext *rtpctx;

    int ret;

    AVOutputFormat *rtp_format = av_guess_format("rtp", NULL, NULL);



    if (!rtp_format)

        return NULL;



    

    rtpctx = avformat_alloc_context();

    if (!rtpctx)

        return NULL;



    rtpctx->oformat = rtp_format;

    if (!av_new_stream(rtpctx, 0)) {

        av_free(rtpctx);

        return NULL;

    }

    

    rtpctx->max_delay = s->max_delay;

    

    rtpctx->streams[0]->sample_aspect_ratio = st->sample_aspect_ratio;



    

    rtpctx->start_time_realtime = s->start_time_realtime;



    

    av_free(rtpctx->streams[0]->codec);

    rtpctx->streams[0]->codec = st->codec;



    if (handle) {

        url_fdopen(&rtpctx->pb, handle);

    } else

        url_open_dyn_packet_buf(&rtpctx->pb, packet_size);

    ret = av_write_header(rtpctx);



    if (ret) {

        if (handle) {

            url_fclose(rtpctx->pb);

        } else {

            uint8_t *ptr;

            url_close_dyn_buf(rtpctx->pb, &ptr);

            av_free(ptr);

        }


        av_free(rtpctx->streams[0]);

        av_free(rtpctx);

        return NULL;

    }



    

    st->time_base = rtpctx->streams[0]->time_base;

    return rtpctx;

}