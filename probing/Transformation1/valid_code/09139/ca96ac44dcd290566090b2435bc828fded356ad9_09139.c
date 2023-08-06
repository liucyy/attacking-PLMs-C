int aio_bh_poll(AioContext *ctx)

{

    QEMUBH *bh, **bhp, *next;

    int ret;



    ctx->walking_bh++;



    ret = 0;

    for (bh = ctx->first_bh; bh; bh = next) {

        

        smp_read_barrier_depends();

        next = bh->next;

        

        if (!bh->deleted && atomic_xchg(&bh->scheduled, 0)) {

            if (!bh->idle)

                ret = 1;

            bh->idle = 0;

            bh->cb(bh->opaque);

        }

    }



    ctx->walking_bh--;



    

    if (!ctx->walking_bh) {

        qemu_mutex_lock(&ctx->bh_lock);

        bhp = &ctx->first_bh;

        while (*bhp) {

            bh = *bhp;

            if (bh->deleted) {

                *bhp = bh->next;

                g_free(bh);

            } else {

                bhp = &bh->next;

            }

        }

        qemu_mutex_unlock(&ctx->bh_lock);

    }



    return ret;

}