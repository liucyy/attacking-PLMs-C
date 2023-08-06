static bool coroutine_fn yield_and_check(BackupBlockJob *job)

{

    if (block_job_is_cancelled(&job->common)) {

        return true;

    }



    

    if (job->common.speed) {

        uint64_t delay_ns = ratelimit_calculate_delay(&job->limit,

                                                      job->sectors_read);

        job->sectors_read = 0;

        block_job_sleep_ns(&job->common, QEMU_CLOCK_REALTIME, delay_ns);

    } else {

        block_job_sleep_ns(&job->common, QEMU_CLOCK_REALTIME, 0);

    }



    if (block_job_is_cancelled(&job->common)) {

        return true;

    }



    return false;

}