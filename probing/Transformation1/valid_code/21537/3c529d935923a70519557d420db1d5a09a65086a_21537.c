static void raw_aio_cancel(BlockDriverAIOCB *blockacb)

{

    int ret;

    RawAIOCB *acb = (RawAIOCB *)blockacb;

    RawAIOCB **pacb;



    ret = aio_cancel(acb->aiocb.aio_fildes, &acb->aiocb);

    if (ret == AIO_NOTCANCELED) {

        

        while (aio_error(&acb->aiocb) == EINPROGRESS);

    }



    

    pacb = &posix_aio_state->first_aio;

    for(;;) {

        if (*pacb == NULL) {

            break;

        } else if (*pacb == acb) {

            *pacb = acb->next;

            raw_fd_pool_put(acb);

            qemu_aio_release(acb);

            break;

        }

        pacb = &acb->next;

    }

}