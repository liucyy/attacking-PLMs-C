bool aio_poll(AioContext *ctx, bool blocking)

{

    AioHandler *node;

    int i;

    int ret = 0;

    bool progress;

    int64_t timeout;

    int64_t start = 0;



    

    if (blocking) {

        atomic_add(&ctx->notify_me, 2);

    }



    qemu_lockcnt_inc(&ctx->list_lock);



    if (ctx->poll_max_ns) {

        start = qemu_clock_get_ns(QEMU_CLOCK_REALTIME);

    }



    progress = try_poll_mode(ctx, blocking);

    if (!progress) {

        assert(npfd == 0);



        



        if (!aio_epoll_enabled(ctx)) {

            QLIST_FOREACH_RCU(node, &ctx->aio_handlers, node) {

                if (!node->deleted && node->pfd.events

                    && aio_node_check(ctx, node->is_external)) {

                    add_pollfd(node);

                }

            }

        }



        timeout = blocking ? aio_compute_timeout(ctx) : 0;



        

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

    }



    if (blocking) {

        atomic_sub(&ctx->notify_me, 2);

    }



    

    if (ctx->poll_max_ns) {

        int64_t block_ns = qemu_clock_get_ns(QEMU_CLOCK_REALTIME) - start;



        if (block_ns <= ctx->poll_ns) {

            

        } else if (block_ns > ctx->poll_max_ns) {

            

            int64_t old = ctx->poll_ns;



            if (ctx->poll_shrink) {

                ctx->poll_ns /= ctx->poll_shrink;

            } else {

                ctx->poll_ns = 0;

            }



            trace_poll_shrink(ctx, old, ctx->poll_ns);

        } else if (ctx->poll_ns < ctx->poll_max_ns &&

                   block_ns < ctx->poll_max_ns) {

            

            int64_t old = ctx->poll_ns;

            int64_t grow = ctx->poll_grow;



            if (grow == 0) {

                grow = 2;

            }



            if (ctx->poll_ns) {

                ctx->poll_ns *= grow;

            } else {

                ctx->poll_ns = 4000; 

            }



            if (ctx->poll_ns > ctx->poll_max_ns) {

                ctx->poll_ns = ctx->poll_max_ns;

            }



            trace_poll_grow(ctx, old, ctx->poll_ns);

        }

    }



    aio_notify_accept(ctx);



    

    if (ret > 0) {

        for (i = 0; i < npfd; i++) {

            nodes[i]->pfd.revents = pollfds[i].revents;

        }

    }



    npfd = 0;

    qemu_lockcnt_dec(&ctx->list_lock);



    

    if (aio_dispatch(ctx, ret > 0)) {

        progress = true;

    }



    return progress;

}