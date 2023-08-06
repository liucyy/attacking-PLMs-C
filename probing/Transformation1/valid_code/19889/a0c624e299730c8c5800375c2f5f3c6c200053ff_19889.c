int ff_v4l2_m2m_codec_reinit(V4L2m2mContext* s)

{

    int ret;



    av_log(s->avctx, AV_LOG_DEBUG, "reinit context\n");



    

    ret = ff_v4l2_context_set_status(&s->capture, VIDIOC_STREAMOFF);

    if (ret)

        av_log(s->avctx, AV_LOG_ERROR, "capture VIDIOC_STREAMOFF\n");



    

    av_log(s->avctx, AV_LOG_DEBUG, "waiting for user to release AVBufferRefs\n");

    if (atomic_load(&s->refcount))

        while(sem_wait(&s->refsync) == -1 && errno == EINTR);



    ff_v4l2_context_release(&s->capture);



    

    ret = ff_v4l2_context_get_format(&s->capture);

    if (ret) {

        av_log(s->avctx, AV_LOG_ERROR, "query the new capture format\n");

        return ret;

    }



    

    ret = ff_v4l2_context_set_format(&s->capture);

    if (ret) {

        av_log(s->avctx, AV_LOG_ERROR, "setting capture format\n");

        return ret;

    }



    

    sem_destroy(&s->refsync);

    sem_init(&s->refsync, 0, 0);

    s->draining = 0;

    s->reinit = 0;



    return 0;

}