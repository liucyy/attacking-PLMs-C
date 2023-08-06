static bool aio_epoll_enabled(AioContext *ctx)

{

    

    return !aio_external_disabled(ctx) && ctx->epoll_enabled;

}