static int mmap_read_frame(AVFormatContext *ctx, AVPacket *pkt)

{

    struct video_data *s = ctx->priv_data;

    struct v4l2_buffer buf = {

        .type   = V4L2_BUF_TYPE_VIDEO_CAPTURE,

        .memory = V4L2_MEMORY_MMAP

    };

    int res;



    pkt->size = 0;



    

    while ((res = v4l2_ioctl(s->fd, VIDIOC_DQBUF, &buf)) < 0 && (errno == EINTR));

    if (res < 0) {

        if (errno == EAGAIN)

            return AVERROR(EAGAIN);



        res = AVERROR(errno);

        av_log(ctx, AV_LOG_ERROR, "ioctl(VIDIOC_DQBUF): %s\n",

               av_err2str(res));

        return res;

    }



    if (buf.index >= s->buffers) {

        av_log(ctx, AV_LOG_ERROR, "Invalid buffer index received.\n");

        return AVERROR(EINVAL);

    }

    atomic_fetch_add(&s->buffers_queued, -1);

    

    av_assert0(atomic_load(&s->buffers_queued) >= 1);



#ifdef V4L2_BUF_FLAG_ERROR

    if (buf.flags & V4L2_BUF_FLAG_ERROR) {

        av_log(ctx, AV_LOG_WARNING,

               "Dequeued v4l2 buffer contains corrupted data (%d bytes).\n",

               buf.bytesused);

        buf.bytesused = 0;

    } else

#endif

    {

        

        if (ctx->video_codec_id == AV_CODEC_ID_CPIA)

            s->frame_size = buf.bytesused;



        if (s->frame_size > 0 && buf.bytesused != s->frame_size) {

            av_log(ctx, AV_LOG_ERROR,

                   "Dequeued v4l2 buffer contains %d bytes, but %d were expected. Flags: 0x%08X.\n",

                   buf.bytesused, s->frame_size, buf.flags);

            enqueue_buffer(s, &buf);

            return AVERROR_INVALIDDATA;

        }

    }



    

    if (atomic_load(&s->buffers_queued) == FFMAX(s->buffers / 8, 1)) {

        

        res = av_new_packet(pkt, buf.bytesused);

        if (res < 0) {

            av_log(ctx, AV_LOG_ERROR, "Error allocating a packet.\n");

            enqueue_buffer(s, &buf);

            return res;

        }

        memcpy(pkt->data, s->buf_start[buf.index], buf.bytesused);



        res = enqueue_buffer(s, &buf);

        if (res) {

            av_packet_unref(pkt);

            return res;

        }

    } else {

        struct buff_data *buf_descriptor;



        pkt->data     = s->buf_start[buf.index];

        pkt->size     = buf.bytesused;



        buf_descriptor = av_malloc(sizeof(struct buff_data));

        if (!buf_descriptor) {

            

            av_log(ctx, AV_LOG_ERROR, "Failed to allocate a buffer descriptor\n");

            enqueue_buffer(s, &buf);



            return AVERROR(ENOMEM);

        }

        buf_descriptor->index = buf.index;

        buf_descriptor->s     = s;



        pkt->buf = av_buffer_create(pkt->data, pkt->size, mmap_release_buffer,

                                    buf_descriptor, 0);

        if (!pkt->buf) {

            av_log(ctx, AV_LOG_ERROR, "Failed to create a buffer\n");

            enqueue_buffer(s, &buf);

            av_freep(&buf_descriptor);

            return AVERROR(ENOMEM);

        }

    }

    pkt->pts = buf.timestamp.tv_sec * INT64_C(1000000) + buf.timestamp.tv_usec;

    convert_timestamp(ctx, &pkt->pts);



    return pkt->size;

}