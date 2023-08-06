void aio_bh_update_timeout(AioContext *ctx, uint32_t *timeout)

{

    QEMUBH *bh;



    for (bh = ctx->first_bh; bh; bh = bh->next) {

        if (!bh->deleted && bh->scheduled) {

            if (bh->idle) {

                

                *timeout = MIN(10, *timeout);

            } else {

                

                *timeout = 0;

                break;

            }

        }

    }

}