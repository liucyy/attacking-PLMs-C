static V4L2Buffer* v4l2_dequeue_v4l2buf(V4L2Context *ctx, int timeout)

{

    struct v4l2_plane planes[VIDEO_MAX_PLANES];

    struct v4l2_buffer buf = { 0 };

    V4L2Buffer* avbuf = NULL;

    struct pollfd pfd = {

        .events =  POLLIN | POLLRDNORM | POLLPRI | POLLOUT | POLLWRNORM, 

        .fd = ctx_to_m2mctx(ctx)->fd,

    };

    int ret;



    if (V4L2_TYPE_IS_OUTPUT(ctx->type))

        pfd.events =  POLLOUT | POLLWRNORM;



    for (;;) {

        ret = poll(&pfd, 1, timeout);

        if (ret > 0)

            break;

        if (errno == EINTR)

            continue;



        

        if (ctx_to_m2mctx(ctx)->draining)

            ctx->done = 1;



        return NULL;

    }



    

    if (pfd.revents & POLLERR) {

        av_log(logger(ctx), AV_LOG_WARNING, "%s POLLERR\n", ctx->name);

        return NULL;

    }



    

    if (pfd.revents & POLLPRI) {

        ret = v4l2_handle_event(ctx);

        if (ret < 0) {

            

            ctx->done = EINVAL;

            return NULL;

        }

        if (ret) {

            

            return NULL;

        }

    }



    

    if (pfd.revents & (POLLIN | POLLRDNORM | POLLOUT | POLLWRNORM)) {



        if (!V4L2_TYPE_IS_OUTPUT(ctx->type)) {

            

            if (pfd.revents & (POLLIN | POLLRDNORM))

                goto dequeue;



            

            if (pfd.revents & (POLLOUT | POLLWRNORM))

                return NULL;

        }



dequeue:

        memset(&buf, 0, sizeof(buf));

        buf.memory = V4L2_MEMORY_MMAP;

        buf.type = ctx->type;

        if (V4L2_TYPE_IS_MULTIPLANAR(ctx->type)) {

            memset(planes, 0, sizeof(planes));

            buf.length = VIDEO_MAX_PLANES;

            buf.m.planes = planes;

        }



        ret = ioctl(ctx_to_m2mctx(ctx)->fd, VIDIOC_DQBUF, &buf);

        if (ret) {

            if (errno != EAGAIN) {

                ctx->done = errno;

                if (errno != EPIPE)

                    av_log(logger(ctx), AV_LOG_DEBUG, "%s VIDIOC_DQBUF, errno (%s)\n",

                        ctx->name, av_err2str(AVERROR(errno)));

            }

        } else {

            avbuf = &ctx->buffers[buf.index];

            avbuf->status = V4L2BUF_AVAILABLE;

            avbuf->buf = buf;

            if (V4L2_TYPE_IS_MULTIPLANAR(ctx->type)) {

                memcpy(avbuf->planes, planes, sizeof(planes));

                avbuf->buf.m.planes = avbuf->planes;

            }

        }

    }



    return avbuf;

}