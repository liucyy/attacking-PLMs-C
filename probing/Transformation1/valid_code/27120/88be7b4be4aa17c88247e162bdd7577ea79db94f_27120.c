BdrvNextIterator *bdrv_next(BdrvNextIterator *it, BlockDriverState **bs)

{

    if (!it) {

        it = g_new(BdrvNextIterator, 1);

        *it = (BdrvNextIterator) {

            .phase = BDRV_NEXT_BACKEND_ROOTS,

        };

    }



    

    if (it->phase == BDRV_NEXT_BACKEND_ROOTS) {

        do {

            it->blk = blk_all_next(it->blk);

            *bs = it->blk ? blk_bs(it->blk) : NULL;

        } while (it->blk && (*bs == NULL || bdrv_first_blk(*bs) != it->blk));



        if (*bs) {

            return it;

        }

        it->phase = BDRV_NEXT_MONITOR_OWNED;

    }



    

    do {

        it->bs = bdrv_next_monitor_owned(it->bs);

        *bs = it->bs;

    } while (*bs && bdrv_has_blk(*bs));



    return *bs ? it : NULL;

}