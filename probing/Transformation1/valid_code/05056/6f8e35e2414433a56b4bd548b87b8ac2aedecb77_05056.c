static int coroutine_fn backup_run_incremental(BackupBlockJob *job)

{

    bool error_is_read;

    int ret = 0;

    int clusters_per_iter;

    uint32_t granularity;

    int64_t sector;

    int64_t cluster;

    int64_t end;

    int64_t last_cluster = -1;

    int64_t sectors_per_cluster = cluster_size_sectors(job);

    BdrvDirtyBitmapIter *dbi;



    granularity = bdrv_dirty_bitmap_granularity(job->sync_bitmap);

    clusters_per_iter = MAX((granularity / job->cluster_size), 1);

    dbi = bdrv_dirty_iter_new(job->sync_bitmap, 0);



    

    while ((sector = bdrv_dirty_iter_next(dbi)) != -1) {

        cluster = sector / sectors_per_cluster;



        

        if (cluster != last_cluster + 1) {

            job->common.offset += ((cluster - last_cluster - 1) *

                                   job->cluster_size);

        }



        for (end = cluster + clusters_per_iter; cluster < end; cluster++) {

            do {

                if (yield_and_check(job)) {

                    goto out;

                }

                ret = backup_do_cow(job, cluster * job->cluster_size,

                                    job->cluster_size, &error_is_read,

                                    false);

                if ((ret < 0) &&

                    backup_error_action(job, error_is_read, -ret) ==

                    BLOCK_ERROR_ACTION_REPORT) {

                    goto out;

                }

            } while (ret < 0);

        }



        

        if (granularity < job->cluster_size) {

            bdrv_set_dirty_iter(dbi, cluster * sectors_per_cluster);

        }



        last_cluster = cluster - 1;

    }



    

    end = DIV_ROUND_UP(job->common.len, job->cluster_size);

    if (last_cluster + 1 < end) {

        job->common.offset += ((end - last_cluster - 1) * job->cluster_size);

    }



out:

    bdrv_dirty_iter_free(dbi);

    return ret;

}