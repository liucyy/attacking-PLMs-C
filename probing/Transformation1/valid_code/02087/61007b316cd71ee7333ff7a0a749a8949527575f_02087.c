static void bdrv_io_limits_intercept(BlockDriverState *bs,

                                     unsigned int bytes,

                                     bool is_write)

{

    

    bool must_wait = throttle_schedule_timer(&bs->throttle_state, is_write);



    

    if (must_wait ||

        !qemu_co_queue_empty(&bs->throttled_reqs[is_write])) {

        qemu_co_queue_wait(&bs->throttled_reqs[is_write]);

    }



    

    throttle_account(&bs->throttle_state, is_write, bytes);





    

    if (throttle_schedule_timer(&bs->throttle_state, is_write)) {

        return;

    }



    

    qemu_co_queue_next(&bs->throttled_reqs[is_write]);

}