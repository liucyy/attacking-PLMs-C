void ff_af_queue_close(AudioFrameQueue *afq)

{

    

    while (afq->frame_queue)

        delete_next_frame(afq);

    memset(afq, 0, sizeof(*afq));

}