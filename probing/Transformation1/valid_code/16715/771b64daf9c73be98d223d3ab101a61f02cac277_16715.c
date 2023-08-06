static void laio_cancel(BlockDriverAIOCB *blockacb)

{

    struct qemu_laiocb *laiocb = (struct qemu_laiocb *)blockacb;

    struct io_event event;

    int ret;



    if (laiocb->ret != -EINPROGRESS)

        return;



    

    ret = io_cancel(laiocb->ctx->ctx, &laiocb->iocb, &event);

    if (ret == 0) {

        laiocb->ret = -ECANCELED;

        return;

    }



    

    while (laiocb->ret == -EINPROGRESS) {

        qemu_laio_completion_cb(&laiocb->ctx->e);

    }

}