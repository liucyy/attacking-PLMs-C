static bool bdrv_drain_recurse(BlockDriverState *bs, bool begin)

{

    BdrvChild *child, *tmp;

    bool waited;



    

    bdrv_drain_invoke(bs, begin);



    

    waited = BDRV_POLL_WHILE(bs, atomic_read(&bs->in_flight) > 0);



    QLIST_FOREACH_SAFE(child, &bs->children, next, tmp) {

        BlockDriverState *bs = child->bs;

        bool in_main_loop =

            qemu_get_current_aio_context() == qemu_get_aio_context();

        assert(bs->refcnt > 0);

        if (in_main_loop) {

            

            bdrv_ref(bs);

        }

        waited |= bdrv_drain_recurse(bs, begin);

        if (in_main_loop) {

            bdrv_unref(bs);

        }

    }



    return waited;

}