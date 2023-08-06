static void coroutine_fn backup_run(void *opaque)

{

    BackupBlockJob *job = opaque;

    BackupCompleteData *data;

    BlockDriverState *bs = job->common.bs;

    BlockDriverState *target = job->target;

    BlockdevOnError on_target_error = job->on_target_error;

    NotifierWithReturn before_write = {

        .notify = backup_before_write_notify,

    };

    int64_t start, end;

    int ret = 0;



    QLIST_INIT(&job->inflight_reqs);

    qemu_co_rwlock_init(&job->flush_rwlock);



    start = 0;

    end = DIV_ROUND_UP(job->common.len / BDRV_SECTOR_SIZE,

                       BACKUP_SECTORS_PER_CLUSTER);



    job->bitmap = hbitmap_alloc(end, 0);



    bdrv_set_enable_write_cache(target, true);

    bdrv_set_on_error(target, on_target_error, on_target_error);

    bdrv_iostatus_enable(target);



    bdrv_add_before_write_notifier(bs, &before_write);



    if (job->sync_mode == MIRROR_SYNC_MODE_NONE) {

        while (!block_job_is_cancelled(&job->common)) {

            

            job->common.busy = false;

            qemu_coroutine_yield();

            job->common.busy = true;

        }

    } else {

        

        for (; start < end; start++) {

            bool error_is_read;



            if (block_job_is_cancelled(&job->common)) {

                break;

            }



            

            if (job->common.speed) {

                uint64_t delay_ns = ratelimit_calculate_delay(

                        &job->limit, job->sectors_read);

                job->sectors_read = 0;

                block_job_sleep_ns(&job->common, QEMU_CLOCK_REALTIME, delay_ns);

            } else {

                block_job_sleep_ns(&job->common, QEMU_CLOCK_REALTIME, 0);

            }



            if (block_job_is_cancelled(&job->common)) {

                break;

            }



            if (job->sync_mode == MIRROR_SYNC_MODE_TOP) {

                int i, n;

                int alloced = 0;



                



                for (i = 0; i < BACKUP_SECTORS_PER_CLUSTER;) {

                    

                    alloced =

                        bdrv_is_allocated(bs,

                                start * BACKUP_SECTORS_PER_CLUSTER + i,

                                BACKUP_SECTORS_PER_CLUSTER - i, &n);

                    i += n;



                    if (alloced == 1 || n == 0) {

                        break;

                    }

                }



                

                if (alloced == 0) {

                    continue;

                }

            }

            

            ret = backup_do_cow(bs, start * BACKUP_SECTORS_PER_CLUSTER,

                    BACKUP_SECTORS_PER_CLUSTER, &error_is_read);

            if (ret < 0) {

                

                BlockErrorAction action =

                    backup_error_action(job, error_is_read, -ret);

                if (action == BLOCK_ERROR_ACTION_REPORT) {

                    break;

                } else {

                    start--;

                    continue;

                }

            }

        }

    }



    notifier_with_return_remove(&before_write);



    

    qemu_co_rwlock_wrlock(&job->flush_rwlock);

    qemu_co_rwlock_unlock(&job->flush_rwlock);



    hbitmap_free(job->bitmap);



    bdrv_iostatus_disable(target);




    data = g_malloc(sizeof(*data));

    data->ret = ret;

    block_job_defer_to_main_loop(&job->common, backup_complete, data);

}