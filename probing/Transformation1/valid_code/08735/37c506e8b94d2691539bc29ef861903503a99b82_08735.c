static int mmsh_open(URLContext *h, const char *uri, int flags)

{

    int i, port, err;

    char httpname[256], path[256], host[128], location[1024];

    char *stream_selection;

    char headers[1024];

    MMSHContext *mmsh;

    MMSContext *mms;



    mmsh = h->priv_data = av_mallocz(sizeof(MMSHContext));

    if (!h->priv_data)

        return AVERROR(ENOMEM);

    mmsh->request_seq = h->is_streamed = 1;

    mms = &mmsh->mms;

    av_strlcpy(location, uri, sizeof(location));



    av_url_split(NULL, 0, NULL, 0,

        host, sizeof(host), &port, path, sizeof(path), location);

    if (port<0)

        port = 80; 

    ff_url_join(httpname, sizeof(httpname), "http", NULL, host, port, path);



    if (url_alloc(&mms->mms_hd, httpname, URL_RDONLY) < 0) {

        return AVERROR(EIO);

    }



    snprintf(headers, sizeof(headers),

             "Accept: **\r\n"

                   USERAGENT

                   "Host: %s:%d\r\n"

                   "Pragma: no-cache,rate=1.000000,request-context=%u\r\n"

                   "Pragma: xPlayStrm=1\r\n"

                   CLIENTGUID

                   "Pragma: stream-switch-count=%d\r\n"

                   "Pragma: stream-switch-entry=%s\r\n"

                   "Connection: Close\r\n\r\n",

                   host, port, mmsh->request_seq++, mms->stream_num, stream_selection);

    av_freep(&stream_selection);

    if (err < 0) {

        av_log(NULL, AV_LOG_ERROR, "Build play request failed!\n");

        goto fail;

    }

    dprintf(NULL, "out_buffer is %s", headers);

    ff_http_set_headers(mms->mms_hd, headers);



    err = url_connect(mms->mms_hd);

    if (err) {

          goto fail;

    }



    err = get_http_header_data(mmsh);

    if (err) {

        av_log(NULL, AV_LOG_ERROR, "Get http header data failed!\n");

        goto fail;

    }



    dprintf(NULL, "Connection successfully open\n");

    return 0;

fail:

    av_freep(&stream_selection);

    mmsh_close(h);

    dprintf(NULL, "Connection failed with error %d\n", err);

    return err;

}