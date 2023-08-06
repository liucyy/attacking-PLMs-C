static int v4l2_read_header(AVFormatContext *ctx)

{

    struct video_data *s = ctx->priv_data;

    AVStream *st;

    int res = 0;

    uint32_t desired_format;

    enum AVCodecID codec_id = AV_CODEC_ID_NONE;

    enum AVPixelFormat pix_fmt = AV_PIX_FMT_NONE;

    struct v4l2_input input = { 0 };



    st = avformat_new_stream(ctx, NULL);

    if (!st)

        return AVERROR(ENOMEM);



#if CONFIG_LIBV4L2

    

    if (s->use_libv4l2)

        v4l2_log_file = fopen("/dev/null", "w");

#endif



    s->fd = device_open(ctx);

    if (s->fd < 0)

        return s->fd;



    if (s->channel != -1) {

        

        av_log(ctx, AV_LOG_DEBUG, "Selecting input_channel: %d\n", s->channel);

        if (v4l2_ioctl(s->fd, VIDIOC_S_INPUT, &s->channel) < 0) {

            res = AVERROR(errno);

            av_log(ctx, AV_LOG_ERROR, "ioctl(VIDIOC_S_INPUT): %s\n", av_err2str(res));

            goto fail;

        }

    } else {

        

        if (v4l2_ioctl(s->fd, VIDIOC_G_INPUT, &s->channel) < 0) {

            res = AVERROR(errno);

            av_log(ctx, AV_LOG_ERROR, "ioctl(VIDIOC_G_INPUT): %s\n", av_err2str(res));

            goto fail;

        }

    }



    

    input.index = s->channel;

    if (v4l2_ioctl(s->fd, VIDIOC_ENUMINPUT, &input) < 0) {

        res = AVERROR(errno);

        av_log(ctx, AV_LOG_ERROR, "ioctl(VIDIOC_ENUMINPUT): %s\n", av_err2str(res));

        goto fail;

    }

    s->std_id = input.std;

    av_log(ctx, AV_LOG_DEBUG, "Current input_channel: %d, input_name: %s, input_std: %"PRIx64"\n",

           s->channel, input.name, (uint64_t)input.std);



    if (s->list_format) {

        list_formats(ctx, s->list_format);

        res = AVERROR_EXIT;

        goto fail;

    }



    if (s->list_standard) {

        list_standards(ctx);

        res = AVERROR_EXIT;

        goto fail;

    }



    avpriv_set_pts_info(st, 64, 1, 1000000); 



    if ((res = v4l2_set_parameters(ctx)) < 0)

        goto fail;



    if (s->pixel_format) {

        AVCodec *codec = avcodec_find_decoder_by_name(s->pixel_format);



        if (codec)

            ctx->video_codec_id = codec->id;



        pix_fmt = av_get_pix_fmt(s->pixel_format);



        if (pix_fmt == AV_PIX_FMT_NONE && !codec) {

            av_log(ctx, AV_LOG_ERROR, "No such input format: %s.\n",

                   s->pixel_format);



            res = AVERROR(EINVAL);

            goto fail;

        }

    }



    if (!s->width && !s->height) {

        struct v4l2_format fmt = { .type = V4L2_BUF_TYPE_VIDEO_CAPTURE };



        av_log(ctx, AV_LOG_VERBOSE,

               "Querying the device for the current frame size\n");

        if (v4l2_ioctl(s->fd, VIDIOC_G_FMT, &fmt) < 0) {

            res = AVERROR(errno);

            av_log(ctx, AV_LOG_ERROR, "ioctl(VIDIOC_G_FMT): %s\n", av_err2str(res));

            goto fail;

        }



        s->width  = fmt.fmt.pix.width;

        s->height = fmt.fmt.pix.height;

        av_log(ctx, AV_LOG_VERBOSE,

               "Setting frame size to %dx%d\n", s->width, s->height);

    }



    res = device_try_init(ctx, pix_fmt, &s->width, &s->height, &desired_format, &codec_id);

    if (res < 0)

        goto fail;



    

    if (codec_id != AV_CODEC_ID_NONE && ctx->video_codec_id == AV_CODEC_ID_NONE)

        ctx->video_codec_id = codec_id;



    if ((res = av_image_check_size(s->width, s->height, 0, ctx)) < 0)

        goto fail;



    s->frame_format = desired_format;



    st->codec->pix_fmt = avpriv_fmt_v4l2ff(desired_format, codec_id);

    s->frame_size =

        avpicture_get_size(st->codec->pix_fmt, s->width, s->height);



    if ((res = mmap_init(ctx)) ||

        (res = mmap_start(ctx)) < 0)

            goto fail;



    s->top_field_first = first_field(s);



    st->codec->codec_type = AVMEDIA_TYPE_VIDEO;

    st->codec->codec_id = codec_id;

    if (codec_id == AV_CODEC_ID_RAWVIDEO)

        st->codec->codec_tag =

            avcodec_pix_fmt_to_codec_tag(st->codec->pix_fmt);

    else if (codec_id == AV_CODEC_ID_H264) {

        st->need_parsing = AVSTREAM_PARSE_HEADERS;

    }

    if (desired_format == V4L2_PIX_FMT_YVU420)

        st->codec->codec_tag = MKTAG('Y', 'V', '1', '2');

    else if (desired_format == V4L2_PIX_FMT_YVU410)

        st->codec->codec_tag = MKTAG('Y', 'V', 'U', '9');

    st->codec->width = s->width;

    st->codec->height = s->height;

    if (st->avg_frame_rate.den)

        st->codec->bit_rate = s->frame_size * av_q2d(st->avg_frame_rate) * 8;



    return 0;



fail:

    v4l2_close(s->fd);

    return res;

}