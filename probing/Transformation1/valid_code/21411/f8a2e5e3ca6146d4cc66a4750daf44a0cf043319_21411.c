static int qcow2_is_allocated(BlockDriverState *bs, int64_t sector_num,

                              int nb_sectors, int *pnum)

{

    uint64_t cluster_offset;

    int ret;



    *pnum = nb_sectors;

    

    ret = qcow2_get_cluster_offset(bs, sector_num << 9, pnum, &cluster_offset);

    if (ret < 0) {

        *pnum = 0;

    }



    return (cluster_offset != 0);

}