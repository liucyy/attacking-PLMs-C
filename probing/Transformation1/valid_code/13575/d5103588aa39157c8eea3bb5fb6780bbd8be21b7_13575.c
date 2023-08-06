static void bdrv_io_limits_intercept(BlockDriverState *bs,

                                     int nb_sectors,

                                     bool is_write)

{

    

    bool must_wait = throttle_schedule_timer(&bs->throttle_state, is_write);



    

    if (must_wait ||

        !qemu_co_queue_empty(&bs->throttled_reqs[is_write])) {

        qemu_co_queue_wait(&bs->throttled_reqs[is_write]);

    }



    

    throttle_account(&bs->throttle_state,

                     is_write,

                     nb_sectors * BDRV_SECTOR_SIZE);



    

    if (throttle_schedule_timer(&bs->throttle_state, is_write)) {

        return;

    }



    

    qemu_co_queue_next(&bs->throttled_reqs[is_write]);

}