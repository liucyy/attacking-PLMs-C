void qemu_bh_schedule(QEMUBH *bh)

{

    AioContext *ctx;



    ctx = bh->ctx;

    bh->idle = 0;

    

    if (atomic_xchg(&bh->scheduled, 1) == 0) {

        aio_notify(ctx);

    }

}