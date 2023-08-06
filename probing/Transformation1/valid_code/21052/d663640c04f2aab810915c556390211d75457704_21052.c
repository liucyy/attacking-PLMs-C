static int coroutine_fn qcow2_co_is_allocated(BlockDriverState *bs,

        int64_t sector_num, int nb_sectors, int *pnum)

{

    BDRVQcowState *s = bs->opaque;

    uint64_t cluster_offset;

    int ret;



    *pnum = nb_sectors;

    

    qemu_co_mutex_lock(&s->lock);

    ret = qcow2_get_cluster_offset(bs, sector_num << 9, pnum, &cluster_offset);

    qemu_co_mutex_unlock(&s->lock);

    if (ret < 0) {

        *pnum = 0;

    }



    return (cluster_offset != 0) || (ret == QCOW2_CLUSTER_ZERO);

}