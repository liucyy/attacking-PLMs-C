static ssize_t handle_aiocb_ioctl(struct qemu_paiocb *aiocb)

{

    int ret;



    ret = ioctl(aiocb->aio_fildes, aiocb->aio_ioctl_cmd, aiocb->aio_ioctl_buf);

    if (ret == -1)

        return -errno;



    

    return aiocb->aio_nbytes;

}