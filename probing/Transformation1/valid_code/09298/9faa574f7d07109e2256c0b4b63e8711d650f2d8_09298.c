static void gluster_finish_aiocb(struct glfs_fd *fd, ssize_t ret, void *arg)

{

    GlusterAIOCB *acb = (GlusterAIOCB *)arg;

    BlockDriverState *bs = acb->common.bs;

    BDRVGlusterState *s = bs->opaque;

    int retval;



    acb->ret = ret;

    retval = qemu_write_full(s->fds[GLUSTER_FD_WRITE], &acb, sizeof(acb));

    if (retval != sizeof(acb)) {

        

        error_report("Gluster failed to notify QEMU about IO completion");



        qemu_mutex_lock_iothread(); 

        acb->common.cb(acb->common.opaque, -EIO);

        qemu_aio_release(acb);

        close(s->fds[GLUSTER_FD_READ]);

        close(s->fds[GLUSTER_FD_WRITE]);

        qemu_aio_set_fd_handler(s->fds[GLUSTER_FD_READ], NULL, NULL, NULL);

        bs->drv = NULL; 

        qemu_mutex_unlock_iothread();

    }

}