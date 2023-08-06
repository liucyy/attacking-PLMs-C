static BlockDriverAIOCB *raw_aio_writev(BlockDriverState *bs,

    int64_t sector_num, QEMUIOVector *qiov, int nb_sectors,

    BlockDriverCompletionFunc *cb, void *opaque)

{

    const uint8_t *first_buf;

    int first_buf_index = 0, i;



    

    for (i = 0; i < qiov->niov; i++) {

        if (qiov->iov[i].iov_len) {

            assert(qiov->iov[i].iov_len >= 512);

            first_buf_index = i;

            break;

        }

    }



    first_buf = qiov->iov[first_buf_index].iov_base;



    if (check_write_unsafe(bs, sector_num, first_buf, nb_sectors)) {

        RawScrubberBounce *b;

        int ret;



        

        ret = raw_write_scrubbed_bootsect(bs, first_buf);

        if (ret < 0) {

            return NULL;

        }



        



        b = qemu_malloc(sizeof(*b));

        b->cb = cb;

        b->opaque = opaque;



        qemu_iovec_init(&b->qiov, qiov->nalloc);

        qemu_iovec_concat(&b->qiov, qiov, qiov->size);



        b->qiov.size -= 512;

        b->qiov.iov[first_buf_index].iov_base += 512;

        b->qiov.iov[first_buf_index].iov_len -= 512;



        return bdrv_aio_writev(bs->file, sector_num + 1, &b->qiov,

                               nb_sectors - 1, raw_aio_writev_scrubbed, b);

    }



    return bdrv_aio_writev(bs->file, sector_num, qiov, nb_sectors, cb, opaque);

}