static bool aio_dispatch_handlers(AioContext *ctx, HANDLE event)

{

    AioHandler *node;

    bool progress = false;



    

    node = QLIST_FIRST(&ctx->aio_handlers);

    while (node) {

        AioHandler *tmp;



        ctx->walking_handlers++;



        if (!node->deleted &&

            (node->pfd.revents || event_notifier_get_handle(node->e) == event) &&

            node->io_notify) {

            node->pfd.revents = 0;

            node->io_notify(node->e);



            

            if (node->e != &ctx->notifier) {

                progress = true;

            }

        }



        tmp = node;

        node = QLIST_NEXT(node, node);



        ctx->walking_handlers--;



        if (!ctx->walking_handlers && tmp->deleted) {

            QLIST_REMOVE(tmp, node);

            g_free(tmp);

        }

    }



    return progress;

}