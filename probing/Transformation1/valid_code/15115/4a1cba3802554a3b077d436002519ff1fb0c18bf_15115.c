bool aio_poll(AioContext *ctx, bool blocking)

{

    AioHandler *node;

    int i, ret;

    bool progress;

    int64_t timeout;



    aio_context_acquire(ctx);

    progress = false;



    

    if (blocking) {

        atomic_add(&ctx->notify_me, 2);

    }



    ctx->walking_handlers++;



    assert(npfd == 0);



    



    if (!aio_epoll_enabled(ctx)) {

        QLIST_FOREACH(node, &ctx->aio_handlers, node) {

            if (!node->deleted && node->pfd.events

                && aio_node_check(ctx, node->is_external)) {

                add_pollfd(node);

            }

        }

    }



    timeout = blocking ? aio_compute_timeout(ctx) : 0;



    

    if (timeout) {

        aio_context_release(ctx);

    }

    if (aio_epoll_check_poll(ctx, pollfds, npfd, timeout)) {

        AioHandler epoll_handler;



        epoll_handler.pfd.fd = ctx->epollfd;

        epoll_handler.pfd.events = G_IO_IN | G_IO_OUT | G_IO_HUP | G_IO_ERR;

        npfd = 0;

        add_pollfd(&epoll_handler);

        ret = aio_epoll(ctx, pollfds, npfd, timeout);

    } else  {

        ret = qemu_poll_ns(pollfds, npfd, timeout);

    }

    if (blocking) {

        atomic_sub(&ctx->notify_me, 2);

    }

    if (timeout) {

        aio_context_acquire(ctx);

    }



    aio_notify_accept(ctx);



    

    if (ret > 0) {

        for (i = 0; i < npfd; i++) {

            nodes[i]->pfd.revents = pollfds[i].revents;

        }

    }



    npfd = 0;

    ctx->walking_handlers--;



    

    if (aio_dispatch(ctx, ret > 0)) {

        progress = true;

    }



    aio_context_release(ctx);



    return progress;

}