static void rtsp_cmd_teardown(HTTPContext *c, const char *url, RTSPHeader *h)

{

    HTTPContext *rtp_c;



    rtp_c = find_rtp_session_with_url(url, h->session_id);

    if (!rtp_c) {

        rtsp_reply_error(c, RTSP_STATUS_SESSION);

        return;

    }



    

    close_connection(rtp_c);



    

    rtsp_reply_header(c, RTSP_STATUS_OK);

    

    url_fprintf(c->pb, "Session: %s\r\n", rtp_c->session_id);

    url_fprintf(c->pb, "\r\n");

}