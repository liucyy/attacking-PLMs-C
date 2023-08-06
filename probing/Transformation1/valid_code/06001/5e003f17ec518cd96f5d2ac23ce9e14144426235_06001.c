int bdrv_inactivate_all(void)

{

    BlockDriverState *bs = NULL;

    BdrvNextIterator it;

    int ret = 0;

    int pass;



    for (bs = bdrv_first(&it); bs; bs = bdrv_next(&it)) {

        aio_context_acquire(bdrv_get_aio_context(bs));

    }



    

    for (pass = 0; pass < 2; pass++) {

        for (bs = bdrv_first(&it); bs; bs = bdrv_next(&it)) {

            ret = bdrv_inactivate_recurse(bs, pass);

            if (ret < 0) {


                goto out;

            }

        }

    }



out:

    for (bs = bdrv_first(&it); bs; bs = bdrv_next(&it)) {

        aio_context_release(bdrv_get_aio_context(bs));

    }



    return ret;

}