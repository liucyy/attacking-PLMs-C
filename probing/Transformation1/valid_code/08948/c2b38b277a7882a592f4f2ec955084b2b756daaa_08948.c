void aio_context_set_poll_params(AioContext *ctx, int64_t max_ns,

                                 int64_t grow, int64_t shrink, Error **errp)

{

    

    ctx->poll_max_ns = max_ns;

    ctx->poll_ns = 0;

    ctx->poll_grow = grow;

    ctx->poll_shrink = shrink;



    aio_notify(ctx);

}