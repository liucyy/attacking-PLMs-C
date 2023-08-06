static int http_connect(URLContext *h, const char *path, const char *hoststr,

                        const char *auth, int *new_location)

{

    HTTPContext *s = h->priv_data;

    int post, err, ch;

    char line[1024], *q;

    char *auth_b64;

    int auth_b64_len = strlen(auth)* 4 / 3 + 12;

    int64_t off = s->off;





    

    post = h->flags & URL_WRONLY;

    auth_b64 = av_malloc(auth_b64_len);

    av_base64_encode(auth_b64, auth_b64_len, auth, strlen(auth));

    snprintf(s->buffer, sizeof(s->buffer),

             "%s %s HTTP/1.1\r\n"

             "User-Agent: %s\r\n"

             "Accept: *

    s->buf_ptr = s->buffer;

    s->buf_end = s->buffer;

    s->line_count = 0;

    s->off = 0;

    s->filesize = -1;

    if (post) {

        return 0;

    }



    

    q = line;

    for(;;) {

        ch = http_getc(s);

        if (ch < 0)

            return AVERROR(EIO);

        if (ch == '\n') {

            

            if (q > line && q[-1] == '\r')

                q--;

            *q = '\0';

#ifdef DEBUG

            printf("header='%s'\n", line);

#endif

            err = process_line(h, line, s->line_count, new_location);

            if (err < 0)

                return err;

            if (err == 0)

                break;

            s->line_count++;

            q = line;

        } else {

            if ((q - line) < sizeof(line) - 1)

                *q++ = ch;

        }

    }



    return (off == s->off) ? 0 : -1;

}