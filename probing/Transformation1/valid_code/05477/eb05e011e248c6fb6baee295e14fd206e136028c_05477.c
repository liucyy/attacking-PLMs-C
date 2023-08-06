static void block_job_defer_to_main_loop_bh(void *opaque)

{

    BlockJobDeferToMainLoopData *data = opaque;

    AioContext *aio_context;



    

    aio_context_acquire(data->aio_context);



    

    aio_context = blk_get_aio_context(data->job->blk);

    if (aio_context != data->aio_context) {

        aio_context_acquire(aio_context);

    }



    data->job->deferred_to_main_loop = false;

    data->fn(data->job, data->opaque);



    if (aio_context != data->aio_context) {

        aio_context_release(aio_context);

    }



    aio_context_release(data->aio_context);



    g_free(data);

}