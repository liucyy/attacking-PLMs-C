static int64_t coroutine_fn bdrv_co_get_block_status_above(BlockDriverState *bs,

        BlockDriverState *base,

        bool want_zero,

        int64_t sector_num,

        int nb_sectors,

        int *pnum,

        BlockDriverState **file)

{

    BlockDriverState *p;

    int64_t ret = 0;

    bool first = true;



    assert(bs != base);

    for (p = bs; p != base; p = backing_bs(p)) {

        ret = bdrv_co_get_block_status(p, want_zero, sector_num, nb_sectors,

                                       pnum, file);

        if (ret < 0) {

            break;

        }

        if (ret & BDRV_BLOCK_ZERO && ret & BDRV_BLOCK_EOF && !first) {

            

            *pnum = nb_sectors;

        }

        if (ret & (BDRV_BLOCK_ZERO | BDRV_BLOCK_DATA)) {

            break;

        }

        

        nb_sectors = MIN(nb_sectors, *pnum);

        first = false;

    }

    return ret;

}