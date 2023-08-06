static void run_block_job(BlockJob *job, Error **errp)

{

    AioContext *aio_context = blk_get_aio_context(job->blk);



    

    aio_context_acquire(aio_context);

    do {

        aio_poll(aio_context, true);

        qemu_progress_print(job->len ?

                            ((float)job->offset / job->len * 100.f) : 0.0f, 0);

    } while (!job->ready);



    block_job_complete_sync(job, errp);

    aio_context_release(aio_context);



    

    qemu_progress_print(100.f, 0);

}