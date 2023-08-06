void qemu_bh_schedule(QEMUBH *bh)

{

    AioContext *ctx;



    if (bh->scheduled)

        return;

    ctx = bh->ctx;

    bh->idle = 0;

    

    smp_mb();

    bh->scheduled = 1;

    aio_notify(ctx);

}