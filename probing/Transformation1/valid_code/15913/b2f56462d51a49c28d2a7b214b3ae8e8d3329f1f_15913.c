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

    int64_t sectors_per_cluster = cluster_size_sectors(job);

    int ret = 0;



    QLIST_INIT(&job->inflight_reqs);

    qemu_co_rwlock_init(&job->flush_rwlock);



    start = 0;

    end = DIV_ROUND_UP(job->common.len, job->cluster_size);



    job->bitmap = hbitmap_alloc(end, 0);



    bdrv_set_enable_write_cache(target, true);

    if (target->blk) {

        blk_set_on_error(target->blk, on_target_error, on_target_error);

        blk_iostatus_enable(target->blk);

    }



    bdrv_add_before_write_notifier(bs, &before_write);



    if (job->sync_mode == MIRROR_SYNC_MODE_NONE) {

        while (!block_job_is_cancelled(&job->common)) {

            

            job->common.busy = false;

            qemu_coroutine_yield();

            job->common.busy = true;

        }

    } else if (job->sync_mode == MIRROR_SYNC_MODE_INCREMENTAL) {

        ret = backup_run_incremental(job);

    } else {

        

        for (; start < end; start++) {

            bool error_is_read;

            if (yield_and_check(job)) {

                break;

            }



            if (job->sync_mode == MIRROR_SYNC_MODE_TOP) {

                int i, n;

                int alloced = 0;



                



                for (i = 0; i < sectors_per_cluster;) {

                    

                    alloced =

                        bdrv_is_allocated(bs,

                                start * sectors_per_cluster + i,

                                sectors_per_cluster - i, &n);

                    i += n;



                    if (alloced == 1 || n == 0) {

                        break;

                    }

                }



                

                if (alloced == 0) {

                    continue;

                }

            }

            

            ret = backup_do_cow(bs, start * sectors_per_cluster,

                                sectors_per_cluster, &error_is_read, false);

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



    if (target->blk) {

        blk_iostatus_disable(target->blk);

    }

    bdrv_op_unblock_all(target, job->common.blocker);



    data = g_malloc(sizeof(*data));

    data->ret = ret;

    block_job_defer_to_main_loop(&job->common, backup_complete, data);

}