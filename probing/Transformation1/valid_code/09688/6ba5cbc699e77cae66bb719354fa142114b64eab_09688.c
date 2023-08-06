static void rtsp_cmd_setup(HTTPContext *c, const char *url, 

                           RTSPHeader *h)

{

    FFStream *stream;

    int stream_index, port;

    char buf[1024];

    char path1[1024];

    const char *path;

    HTTPContext *rtp_c;

    RTSPTransportField *th;

    struct sockaddr_in dest_addr;

    RTSPActionServerSetup setup;

    

    

    url_split(NULL, 0, NULL, 0, NULL, path1, sizeof(path1), url);

    path = path1;

    if (*path == '/')

        path++;



    

    for(stream = first_stream; stream != NULL; stream = stream->next) {

        if (!stream->is_feed && stream->fmt == &rtp_mux) {

            

            if (!strcmp(path, stream->filename)) {

                if (stream->nb_streams != 1) {

                    rtsp_reply_error(c, RTSP_STATUS_AGGREGATE);

                    return;

                }

                stream_index = 0;

                goto found;

            }

                

            for(stream_index = 0; stream_index < stream->nb_streams;

                stream_index++) {

                snprintf(buf, sizeof(buf), "%s/streamid=%d", 

                         stream->filename, stream_index);

                if (!strcmp(path, buf))

                    goto found;

            }

        }

    }

    

    rtsp_reply_error(c, RTSP_STATUS_SERVICE); 

    return;

 found:



    

    if (h->session_id[0] == '\0') {

        snprintf(h->session_id, sizeof(h->session_id), 

                 "%08x%08x", (int)random(), (int)random());

    }



    

    rtp_c = find_rtp_session(h->session_id);

    if (!rtp_c) {

        

        th = find_transport(h, RTSP_PROTOCOL_RTP_UDP);

        if (!th) {

            th = find_transport(h, RTSP_PROTOCOL_RTP_TCP);

            if (!th) {

                rtsp_reply_error(c, RTSP_STATUS_TRANSPORT);

                return;

            }

        }



        rtp_c = rtp_new_connection(&c->from_addr, stream, h->session_id,

                                   th->protocol);

        if (!rtp_c) {

            rtsp_reply_error(c, RTSP_STATUS_BANDWIDTH);

            return;

        }



        

        if (open_input_stream(rtp_c, "") < 0) {

            rtsp_reply_error(c, RTSP_STATUS_INTERNAL);

            return;

        }

    }

    

    

    if (rtp_c->stream != stream) {

        rtsp_reply_error(c, RTSP_STATUS_SERVICE);

        return;

    }

    

    

    if (rtp_c->rtp_ctx[stream_index]) {

        rtsp_reply_error(c, RTSP_STATUS_STATE);

        return;

    }



    

    th = find_transport(h, rtp_c->rtp_protocol);

    if (!th || (th->protocol == RTSP_PROTOCOL_RTP_UDP && 

                th->client_port_min <= 0)) {

        rtsp_reply_error(c, RTSP_STATUS_TRANSPORT);

        return;

    }



    

    setup.transport_option[0] = '\0';

    dest_addr = rtp_c->from_addr;

    dest_addr.sin_port = htons(th->client_port_min);

    

    

    if (ff_rtsp_callback) {

        setup.ipaddr = ntohl(dest_addr.sin_addr.s_addr);

        if (ff_rtsp_callback(RTSP_ACTION_SERVER_SETUP, rtp_c->session_id, 

                             (char *)&setup, sizeof(setup),

                             stream->rtsp_option) < 0) {

            rtsp_reply_error(c, RTSP_STATUS_TRANSPORT);

            return;

        }

        dest_addr.sin_addr.s_addr = htonl(setup.ipaddr);

    }

    

    

    if (rtp_new_av_stream(rtp_c, stream_index, &dest_addr, c) < 0) {

        rtsp_reply_error(c, RTSP_STATUS_TRANSPORT);

        return;

    }



    

    rtsp_reply_header(c, RTSP_STATUS_OK);

    

    url_fprintf(c->pb, "Session: %s\r\n", rtp_c->session_id);



    switch(rtp_c->rtp_protocol) {

    case RTSP_PROTOCOL_RTP_UDP:

        port = rtp_get_local_port(rtp_c->rtp_handles[stream_index]);

        url_fprintf(c->pb, "Transport: RTP/AVP/UDP;unicast;"

                    "client_port=%d-%d;server_port=%d-%d",

                    th->client_port_min, th->client_port_min + 1,

                    port, port + 1);

        break;

    case RTSP_PROTOCOL_RTP_TCP:

        url_fprintf(c->pb, "Transport: RTP/AVP/TCP;interleaved=%d-%d",

                    stream_index * 2, stream_index * 2 + 1);

        break;

    default:

        break;

    }

    if (setup.transport_option[0] != '\0') {

        url_fprintf(c->pb, ";%s", setup.transport_option);

    }

    url_fprintf(c->pb, "\r\n");

    



    url_fprintf(c->pb, "\r\n");

}