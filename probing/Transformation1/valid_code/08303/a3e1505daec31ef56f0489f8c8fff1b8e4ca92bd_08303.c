static int qcow2_make_empty(BlockDriverState *bs)

{

    BDRVQcow2State *s = bs->opaque;

    uint64_t start_sector;

    int sector_step = INT_MAX / BDRV_SECTOR_SIZE;

    int l1_clusters, ret = 0;



    l1_clusters = DIV_ROUND_UP(s->l1_size, s->cluster_size / sizeof(uint64_t));



    if (s->qcow_version >= 3 && !s->snapshots &&

        3 + l1_clusters <= s->refcount_block_size) {

        

        return make_completely_empty(bs);

    }



    

    for (start_sector = 0; start_sector < bs->total_sectors;

         start_sector += sector_step)

    {

        

        ret = qcow2_discard_clusters(bs, start_sector * BDRV_SECTOR_SIZE,

                                     MIN(sector_step,

                                         bs->total_sectors - start_sector),

                                     QCOW2_DISCARD_SNAPSHOT, true);

        if (ret < 0) {

            break;

        }

    }



    return ret;

}