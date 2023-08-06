static int rtsp_write_packet(AVFormatContext *s, AVPacket *pkt)

{

    RTSPState *rt = s->priv_data;

    RTSPStream *rtsp_st;

    fd_set rfds;

    int n, tcp_fd;

    struct timeval tv;

    AVFormatContext *rtpctx;

    int ret;



    tcp_fd = url_get_file_handle(rt->rtsp_hd);



    while (1) {

        FD_ZERO(&rfds);

        FD_SET(tcp_fd, &rfds);

        tv.tv_sec = 0;

        tv.tv_usec = 0;

        n = select(tcp_fd + 1, &rfds, NULL, NULL, &tv);

        if (n <= 0)

            break;

        if (FD_ISSET(tcp_fd, &rfds)) {

            RTSPMessageHeader reply;



            

            ret = ff_rtsp_read_reply(s, &reply, NULL, 1, NULL);

            if (ret < 0)

                return AVERROR(EPIPE);

            if (ret == 1)

                ff_rtsp_skip_packet(s);

            

            if (rt->state != RTSP_STATE_STREAMING)

                return AVERROR(EPIPE);

        }

    }



    if (pkt->stream_index < 0 || pkt->stream_index >= rt->nb_rtsp_streams)

        return AVERROR_INVALIDDATA;

    rtsp_st = rt->rtsp_streams[pkt->stream_index];

    rtpctx = rtsp_st->transport_priv;



    ret = ff_write_chained(rtpctx, 0, pkt, s);

    

    if (!ret && rt->lower_transport == RTSP_LOWER_TRANSPORT_TCP)

        ret = tcp_write_packet(s, rtsp_st);

    return ret;

}