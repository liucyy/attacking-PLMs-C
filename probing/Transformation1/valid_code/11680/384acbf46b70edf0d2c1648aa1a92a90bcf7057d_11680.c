void qemu_bh_update_timeout(int *timeout)

{

    QEMUBH *bh;



    for (bh = async_context->first_bh; bh; bh = bh->next) {

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