static void rtsp_send_cmd_async (AVFormatContext *s,

                          const char *cmd, RTSPMessageHeader *reply,

                          unsigned char **content_ptr)

{

    RTSPState *rt = s->priv_data;

    char buf[4096], buf1[1024];



    rt->seq++;

    av_strlcpy(buf, cmd, sizeof(buf));

    snprintf(buf1, sizeof(buf1), "CSeq: %d\r\n", rt->seq);

    av_strlcat(buf, buf1, sizeof(buf));

    if (rt->session_id[0] != '\0' && !strstr(cmd, "\nIf-Match:")) {

        snprintf(buf1, sizeof(buf1), "Session: %s\r\n", rt->session_id);

        av_strlcat(buf, buf1, sizeof(buf));

    }

    if (rt->auth_b64)

        av_strlcatf(buf, sizeof(buf),

                    "Authorization: Basic %s\r\n",

                    rt->auth_b64);

    av_strlcat(buf, "\r\n", sizeof(buf));



    dprintf(s, "Sending:\n%s--\n", buf);



    url_write(rt->rtsp_hd, buf, strlen(buf));

    rt->last_cmd_time = av_gettime();

}