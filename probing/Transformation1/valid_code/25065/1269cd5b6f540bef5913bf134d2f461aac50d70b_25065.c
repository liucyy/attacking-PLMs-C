int ff_thread_decode_frame(AVCodecContext *avctx,

                           AVFrame *picture, int *got_picture_ptr,

                           AVPacket *avpkt)

{

    FrameThreadContext *fctx = avctx->internal->thread_ctx;

    int finished = fctx->next_finished;

    PerThreadContext *p;

    int err;



    

    async_unlock(fctx);



    



    p = &fctx->threads[fctx->next_decoding];

    err = update_context_from_user(p->avctx, avctx);

    if (err)

        goto finish;

    err = submit_packet(p, avpkt);

    if (err)

        goto finish;



    



    if (fctx->next_decoding > (avctx->thread_count-1-(avctx->codec_id == AV_CODEC_ID_FFV1)))

        fctx->delaying = 0;



    if (fctx->delaying) {

        *got_picture_ptr=0;

        if (avpkt->size) {

            err = avpkt->size;

            goto finish;

        }

    }



    



    do {

        p = &fctx->threads[finished++];



        if (atomic_load(&p->state) != STATE_INPUT_READY) {

            pthread_mutex_lock(&p->progress_mutex);

            while (atomic_load_explicit(&p->state, memory_order_relaxed) != STATE_INPUT_READY)

                pthread_cond_wait(&p->output_cond, &p->progress_mutex);

            pthread_mutex_unlock(&p->progress_mutex);

        }



        av_frame_move_ref(picture, p->frame);

        *got_picture_ptr = p->got_frame;

        picture->pkt_dts = p->avpkt.dts;



        if (p->result < 0)

            err = p->result;



        

        p->got_frame = 0;



        if (finished >= avctx->thread_count) finished = 0;

    } while (!avpkt->size && !*got_picture_ptr && finished != fctx->next_finished);



    update_context_from_thread(avctx, p->avctx, 1);



    if (fctx->next_decoding >= avctx->thread_count) fctx->next_decoding = 0;



    fctx->next_finished = finished;



    

    if (err >= 0)

        err = avpkt->size;

finish:

    async_lock(fctx);

    return err;

}