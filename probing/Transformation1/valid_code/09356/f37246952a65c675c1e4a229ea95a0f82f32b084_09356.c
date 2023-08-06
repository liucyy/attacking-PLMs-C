static int http_connect(URLContext *h, const char *path, const char *hoststr,

                        const char *auth, int *new_location)

{

    HTTPContext *s = h->priv_data;

    int post, err;

    char line[1024];

    char headers[1024] = "";

    char *authstr = NULL;

    int64_t off = s->off;

    int len = 0;





    

    post = h->flags & URL_WRONLY;

    authstr = ff_http_auth_create_response(&s->auth_state, auth, path,

                                        post ? "POST" : "GET");



    

    if (!has_header(s->headers, "\r\nUser-Agent: "))

       len += av_strlcatf(headers + len, sizeof(headers) - len,

                          "User-Agent: %s\r\n", LIBAVFORMAT_IDENT);

    if (!has_header(s->headers, "\r\nAccept: "))

        len += av_strlcpy(headers + len, "Accept: *

    av_strlcpy(headers+len, s->headers, sizeof(headers)-len);



    snprintf(s->buffer, sizeof(s->buffer),

             "%s %s HTTP/1.1\r\n"

             "%s"

             "%s"

             "%s"

             "\r\n",

             post ? "POST" : "GET",

             path,

             post && s->is_chunked ? "Transfer-Encoding: chunked\r\n" : "",

             headers,

             authstr ? authstr : "");



    av_freep(&authstr);

    if (http_write(h, s->buffer, strlen(s->buffer)) < 0)

        return AVERROR(EIO);



    

    s->buf_ptr = s->buffer;

    s->buf_end = s->buffer;

    s->line_count = 0;

    s->off = 0;

    s->filesize = -1;

    if (post) {

        

        s->chunksize = 0;

        

        s->http_code = 200;

        return 0;

    }



    

    for(;;) {

        if (http_get_line(s, line, sizeof(line)) < 0)

            return AVERROR(EIO);



        dprintf(NULL, "header='%s'\n", line);



        err = process_line(h, line, s->line_count, new_location);

        if (err < 0)

            return err;

        if (err == 0)

            break;

        s->line_count++;

    }



    return (off == s->off) ? 0 : -1;

}