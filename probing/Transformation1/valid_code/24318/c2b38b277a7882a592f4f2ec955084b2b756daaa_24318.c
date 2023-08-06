void aio_notify(AioContext *ctx)

{

    

    smp_mb();

    if (ctx->notify_me) {

        event_notifier_set(&ctx->notifier);

        atomic_mb_set(&ctx->notified, true);

    }

}