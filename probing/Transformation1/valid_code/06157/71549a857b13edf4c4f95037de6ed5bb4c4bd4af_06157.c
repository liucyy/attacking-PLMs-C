static int http_connect(URLContext *h, const char *path, const char *local_path,

                        const char *hoststr, const char *auth,

                        const char *proxyauth, int *new_location)

{

    HTTPContext *s = h->priv_data;

    int post, err;

    char headers[1024] = "";

    char *authstr = NULL, *proxyauthstr = NULL;

    int64_t off = s->off;

    int len = 0;

    const char *method;





    

    post = h->flags & AVIO_FLAG_WRITE;



    if (s->post_data) {

        

        post = 1;

        s->chunked_post = 0;

    }



    method = post ? "POST" : "GET";

    authstr = ff_http_auth_create_response(&s->auth_state, auth, local_path,

                                           method);

    proxyauthstr = ff_http_auth_create_response(&s->proxy_auth_state, proxyauth,

                                                local_path, method);



    

    if (!has_header(s->headers, "\r\nUser-Agent: "))

       len += av_strlcatf(headers + len, sizeof(headers) - len,

                          "User-Agent: %s\r\n", LIBAVFORMAT_IDENT);

    if (!has_header(s->headers, "\r\nAccept: "))

        len += av_strlcpy(headers + len, "Accept: *

    if (s->headers)

        av_strlcpy(headers + len, s->headers, sizeof(headers) - len);



    snprintf(s->buffer, sizeof(s->buffer),

             "%s %s HTTP/1.1\r\n"

             "%s"

             "%s"

             "%s"

             "%s%s"

             "\r\n",

             method,

             path,

             post && s->chunked_post ? "Transfer-Encoding: chunked\r\n" : "",

             headers,

             authstr ? authstr : "",

             proxyauthstr ? "Proxy-" : "", proxyauthstr ? proxyauthstr : "");



    av_freep(&authstr);

    av_freep(&proxyauthstr);

    if ((err = ffurl_write(s->hd, s->buffer, strlen(s->buffer))) < 0)

        return err;



    if (s->post_data)

        if ((err = ffurl_write(s->hd, s->post_data, s->post_datalen)) < 0)

            return err;



    

    s->buf_ptr = s->buffer;

    s->buf_end = s->buffer;

    s->line_count = 0;

    s->off = 0;

    s->filesize = -1;

    s->willclose = 0;

    s->end_chunked_post = 0;

    s->end_header = 0;

    if (post && !s->post_data) {

        

        s->http_code = 200;

        return 0;

    }



    

    err = http_read_header(h, new_location);

    if (err < 0)

        return err;



    return (off == s->off) ? 0 : -1;

}