int av_open_input_file(AVFormatContext **ic_ptr, const char *filename,

                       AVInputFormat *fmt,

                       int buf_size,

                       AVFormatParameters *ap)

{

    int err, probe_size;

    AVProbeData probe_data, *pd = &probe_data;

    ByteIOContext *pb = NULL;



    pd->filename = "";

    if (filename)

        pd->filename = filename;

    pd->buf = NULL;

    pd->buf_size = 0;



    if (!fmt) {

        

        fmt = av_probe_input_format(pd, 0);

    }



    

    if (!fmt || !(fmt->flags & AVFMT_NOFILE)) {

        

        if ((err=url_fopen(&pb, filename, URL_RDONLY)) < 0) {

            goto fail;

        }

        if (buf_size > 0) {

            url_setbufsize(pb, buf_size);

        }



        for(probe_size= PROBE_BUF_MIN; probe_size<=PROBE_BUF_MAX && !fmt; probe_size<<=1){

            int score= probe_size < PROBE_BUF_MAX ? AVPROBE_SCORE_MAX/4 : 0;

            

            pd->buf= av_realloc(pd->buf, probe_size + AVPROBE_PADDING_SIZE);

            pd->buf_size = get_buffer(pb, pd->buf, probe_size);

            memset(pd->buf+pd->buf_size, 0, AVPROBE_PADDING_SIZE);

            if (url_fseek(pb, 0, SEEK_SET) < 0) {

                url_fclose(pb);

                if (url_fopen(&pb, filename, URL_RDONLY) < 0) {

                    pb = NULL;

                    err = AVERROR(EIO);

                    goto fail;

                }

            }

            

            fmt = av_probe_input_format2(pd, 1, &score);

        }

        av_freep(&pd->buf);

    }



    

    if (!fmt) {

        err = AVERROR_NOFMT;

        goto fail;

    }



    

    if (fmt->flags & AVFMT_NEEDNUMBER) {

        if (!av_filename_number_test(filename)) {

            err = AVERROR_NUMEXPECTED;

            goto fail;

        }

    }

    err = av_open_input_stream(ic_ptr, pb, filename, fmt, ap);

    if (err)

        goto fail;

    return 0;

 fail:

    av_freep(&pd->buf);

    if (pb)

        url_fclose(pb);

    *ic_ptr = NULL;

    return err;



}