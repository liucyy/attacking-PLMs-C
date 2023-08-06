static void block_job_detach_aio_context(void *opaque)

{

    BlockJob *job = opaque;



    

    block_job_ref(job);



    block_job_pause(job);



    if (!job->paused) {

        

        block_job_enter(job);

    }

    while (!job->paused && !job->completed) {

        aio_poll(block_job_get_aio_context(job), true);

    }



    block_job_unref(job);

}