static int64_t coroutine_fn bdrv_co_get_block_status_above(BlockDriverState *bs,

        BlockDriverState *base,

        int64_t sector_num,

        int nb_sectors,

        int *pnum)

{

    BlockDriverState *p;

    int64_t ret = 0;



    assert(bs != base);

    for (p = bs; p != base; p = backing_bs(p)) {

        ret = bdrv_co_get_block_status(p, sector_num, nb_sectors, pnum);

        if (ret < 0 || ret & BDRV_BLOCK_ALLOCATED) {

            break;

        }

        

        nb_sectors = MIN(nb_sectors, *pnum);

    }

    return ret;

}