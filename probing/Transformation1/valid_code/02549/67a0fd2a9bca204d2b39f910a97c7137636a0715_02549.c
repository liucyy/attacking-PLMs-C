int64_t bdrv_get_block_status_above(BlockDriverState *bs,

                                    BlockDriverState *base,

                                    int64_t sector_num,

                                    int nb_sectors, int *pnum)

{

    Coroutine *co;

    BdrvCoGetBlockStatusData data = {

        .bs = bs,

        .base = base,

        .sector_num = sector_num,

        .nb_sectors = nb_sectors,

        .pnum = pnum,

        .done = false,

    };



    if (qemu_in_coroutine()) {

        

        bdrv_get_block_status_above_co_entry(&data);

    } else {

        AioContext *aio_context = bdrv_get_aio_context(bs);



        co = qemu_coroutine_create(bdrv_get_block_status_above_co_entry);

        qemu_coroutine_enter(co, &data);

        while (!data.done) {

            aio_poll(aio_context, true);

        }

    }

    return data.ret;

}