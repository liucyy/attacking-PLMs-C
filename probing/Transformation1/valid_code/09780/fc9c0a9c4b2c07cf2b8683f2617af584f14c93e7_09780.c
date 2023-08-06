void block_job_sleep_ns(BlockJob *job, QEMUClockType type, int64_t ns)

{

    assert(job->busy);



    

    if (block_job_is_cancelled(job)) {

        return;

    }



    job->busy = false;

    if (!block_job_should_pause(job)) {

        co_aio_sleep_ns(blk_get_aio_context(job->blk), type, ns);

    }

    

    if (block_job_should_pause(job)) {

        qemu_coroutine_yield();

    }

    job->busy = true;

}