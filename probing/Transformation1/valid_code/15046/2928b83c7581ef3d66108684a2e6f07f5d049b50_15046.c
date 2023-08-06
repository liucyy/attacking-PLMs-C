int av_open_input_file(AVFormatContext **ic_ptr, const char *filename,

                       AVInputFormat *fmt,

                       int buf_size,

                       AVFormatParameters *ap)

{

    int err;

    AVProbeData probe_data, *pd = &probe_data;

    ByteIOContext *pb = NULL;

    void *logctx= ap && ap->prealloced_context ? *ic_ptr : NULL;



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

        if ((err = ff_probe_input_buffer(&pb, &fmt, filename, logctx, 0, 0)) < 0) {

            goto fail;

        }

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

    if (ap && ap->prealloced_context)

        av_free(*ic_ptr);

    *ic_ptr = NULL;

    return err;



}