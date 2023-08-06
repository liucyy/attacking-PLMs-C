static int coroutine_fn bdrv_co_do_copy_on_readv(BlockDriverState *bs,

        int64_t offset, unsigned int bytes, QEMUIOVector *qiov)

{

    

    void *bounce_buffer;



    BlockDriver *drv = bs->drv;

    struct iovec iov;

    QEMUIOVector bounce_qiov;

    int64_t cluster_offset;

    unsigned int cluster_bytes;

    size_t skip_bytes;

    int ret;



    

    bdrv_round_to_clusters(bs, offset, bytes, &cluster_offset, &cluster_bytes);



    trace_bdrv_co_do_copy_on_readv(bs, offset, bytes,

                                   cluster_offset, cluster_bytes);



    iov.iov_len = cluster_bytes;

    iov.iov_base = bounce_buffer = qemu_try_blockalign(bs, iov.iov_len);

    if (bounce_buffer == NULL) {

        ret = -ENOMEM;

        goto err;

    }



    qemu_iovec_init_external(&bounce_qiov, &iov, 1);



    ret = bdrv_driver_preadv(bs, cluster_offset, cluster_bytes,

                             &bounce_qiov, 0);

    if (ret < 0) {

        goto err;

    }



    if (drv->bdrv_co_pwrite_zeroes &&

        buffer_is_zero(bounce_buffer, iov.iov_len)) {

        

        ret = bdrv_co_do_pwrite_zeroes(bs, cluster_offset, cluster_bytes, 0);

    } else {

        

        ret = bdrv_driver_pwritev(bs, cluster_offset, cluster_bytes,

                                  &bounce_qiov, 0);

    }



    if (ret < 0) {

        

        goto err;

    }



    skip_bytes = offset - cluster_offset;

    qemu_iovec_from_buf(qiov, 0, bounce_buffer + skip_bytes, bytes);



err:

    qemu_vfree(bounce_buffer);

    return ret;

}