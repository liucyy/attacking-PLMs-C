void aio_notify(AioContext *ctx)

{

    

    smp_mb();

    if (!ctx->dispatching) {

        event_notifier_set(&ctx->notifier);

    }

}