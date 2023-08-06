static int v4l2_try_start(AVCodecContext *avctx)

{

    V4L2m2mContext *s = avctx->priv_data;

    V4L2Context *const capture = &s->capture;

    V4L2Context *const output = &s->output;

    struct v4l2_selection selection;

    int ret;



    

    if (!output->streamon) {

        ret = ff_v4l2_context_set_status(output, VIDIOC_STREAMON);

        if (ret < 0) {

            av_log(avctx, AV_LOG_DEBUG, "VIDIOC_STREAMON on output context\n");

            return ret;

        }

    }



    if (capture->streamon)

        return 0;



    

    capture->format.type = capture->type;

    ret = ioctl(s->fd, VIDIOC_G_FMT, &capture->format);

    if (ret) {

        av_log(avctx, AV_LOG_WARNING, "VIDIOC_G_FMT ioctl\n");

        return ret;

    }



    

    avctx->pix_fmt = ff_v4l2_format_v4l2_to_avfmt(capture->format.fmt.pix_mp.pixelformat, AV_CODEC_ID_RAWVIDEO);

    capture->av_pix_fmt = avctx->pix_fmt;



    

    selection.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    selection.r.height = avctx->coded_height;

    selection.r.width = avctx->coded_width;

    ret = ioctl(s->fd, VIDIOC_S_SELECTION, &selection);

    if (!ret) {

        ret = ioctl(s->fd, VIDIOC_G_SELECTION, &selection);

        if (ret) {

            av_log(avctx, AV_LOG_WARNING, "VIDIOC_G_SELECTION ioctl\n");

        } else {

            av_log(avctx, AV_LOG_DEBUG, "crop output %dx%d\n", selection.r.width, selection.r.height);

            

            capture->height = selection.r.height;

            capture->width  = selection.r.width;

        }

    }



    

    if (!capture->buffers) {

        ret = ff_v4l2_context_init(capture);

        if (ret) {

            av_log(avctx, AV_LOG_DEBUG, "can't request output buffers\n");

            return ret;

        }

    }



    

    ret = ff_v4l2_context_set_status(capture, VIDIOC_STREAMON);

    if (ret) {

        av_log(avctx, AV_LOG_DEBUG, "VIDIOC_STREAMON, on capture context\n");

        return ret;

    }



    return 0;

}