static int queue_picture(VideoState *is, AVFrame *src_frame, double pts1, int64_t pos)

{

    VideoPicture *vp;

    double frame_delay, pts = pts1;



    

    if (pts != 0) {

        

        is->video_clock = pts;

    } else {

        pts = is->video_clock;


    

    frame_delay = av_q2d(is->video_st->codec->time_base);

    

    frame_delay += src_frame->repeat_pict * (frame_delay * 0.5);

    is->video_clock += frame_delay;



#if defined(DEBUG_SYNC) && 0

    printf("frame_type=%c clock=%0.3f pts=%0.3f\n",

           av_get_picture_type_char(src_frame->pict_type), pts, pts1);

#endif



    

    SDL_LockMutex(is->pictq_mutex);



    if(is->pictq_size>=VIDEO_PICTURE_QUEUE_SIZE && !is->refresh)

        is->skip_frames= FFMAX(1.0 - FRAME_SKIP_FACTOR, is->skip_frames * (1.0-FRAME_SKIP_FACTOR));



    while (is->pictq_size >= VIDEO_PICTURE_QUEUE_SIZE &&

           !is->videoq.abort_request) {



    SDL_UnlockMutex(is->pictq_mutex);



    if (is->videoq.abort_request)

        return -1;



    vp = &is->pictq[is->pictq_windex];



    vp->duration = frame_delay;



    

    if (!vp->bmp ||

#if CONFIG_AVFILTER

        vp->width  != is->out_video_filter->inputs[0]->w ||

        vp->height != is->out_video_filter->inputs[0]->h) {

#else

        vp->width != is->video_st->codec->width ||

        vp->height != is->video_st->codec->height) {

#endif

        SDL_Event event;



        vp->allocated = 0;



        

        event.type = FF_ALLOC_EVENT;

        event.user.data1 = is;

        SDL_PushEvent(&event);



        

        SDL_LockMutex(is->pictq_mutex);

        while (!vp->allocated && !is->videoq.abort_request) {









        SDL_UnlockMutex(is->pictq_mutex);



        if (is->videoq.abort_request)

            return -1;




    

    if (vp->bmp) {

        AVPicture pict;

#if CONFIG_AVFILTER

        if(vp->picref)

            avfilter_unref_buffer(vp->picref);

        vp->picref = src_frame->opaque;

#endif



        

        SDL_LockYUVOverlay (vp->bmp);



        memset(&pict,0,sizeof(AVPicture));

        pict.data[0] = vp->bmp->pixels[0];

        pict.data[1] = vp->bmp->pixels[2];

        pict.data[2] = vp->bmp->pixels[1];



        pict.linesize[0] = vp->bmp->pitches[0];

        pict.linesize[1] = vp->bmp->pitches[2];

        pict.linesize[2] = vp->bmp->pitches[1];



#if CONFIG_AVFILTER

        

        av_picture_copy(&pict, (AVPicture *)src_frame,

                        vp->pix_fmt, vp->width, vp->height);

#else

        sws_flags = av_get_int(sws_opts, "sws_flags", NULL);

        is->img_convert_ctx = sws_getCachedContext(is->img_convert_ctx,

            vp->width, vp->height, vp->pix_fmt, vp->width, vp->height,

            PIX_FMT_YUV420P, sws_flags, NULL, NULL, NULL);

        if (is->img_convert_ctx == NULL) {

            fprintf(stderr, "Cannot initialize the conversion context\n");

            exit(1);


        sws_scale(is->img_convert_ctx, src_frame->data, src_frame->linesize,

                  0, vp->height, pict.data, pict.linesize);

#endif

        

        SDL_UnlockYUVOverlay(vp->bmp);



        vp->pts = pts;

        vp->pos = pos;



        

        if (++is->pictq_windex == VIDEO_PICTURE_QUEUE_SIZE)

            is->pictq_windex = 0;

        SDL_LockMutex(is->pictq_mutex);

        vp->target_clock= compute_target_time(vp->pts, is);



        is->pictq_size++;

        SDL_UnlockMutex(is->pictq_mutex);


    return 0;