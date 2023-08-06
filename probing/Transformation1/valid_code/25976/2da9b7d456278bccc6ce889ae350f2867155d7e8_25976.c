void bdrv_drain_all_begin(void)

{

    

    bool waited = true;

    BlockDriverState *bs;

    BdrvNextIterator it;

    GSList *aio_ctxs = NULL, *ctx;



    block_job_pause_all();



    for (bs = bdrv_first(&it); bs; bs = bdrv_next(&it)) {

        AioContext *aio_context = bdrv_get_aio_context(bs);



        aio_context_acquire(aio_context);

        bdrv_parent_drained_begin(bs);

        aio_disable_external(aio_context);

        aio_context_release(aio_context);



        if (!g_slist_find(aio_ctxs, aio_context)) {

            aio_ctxs = g_slist_prepend(aio_ctxs, aio_context);

        }

    }



    

    while (waited) {

        waited = false;



        for (ctx = aio_ctxs; ctx != NULL; ctx = ctx->next) {

            AioContext *aio_context = ctx->data;



            aio_context_acquire(aio_context);

            for (bs = bdrv_first(&it); bs; bs = bdrv_next(&it)) {

                if (aio_context == bdrv_get_aio_context(bs)) {

                    

                    bdrv_drain_invoke(bs, true);

                    waited |= bdrv_drain_recurse(bs, true);

                }

            }

            aio_context_release(aio_context);

        }

    }



    g_slist_free(aio_ctxs);

}