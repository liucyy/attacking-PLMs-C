static int mirror_do_read(MirrorBlockJob *s, int64_t sector_num,

                          int nb_sectors)

{

    BlockBackend *source = s->common.blk;

    int sectors_per_chunk, nb_chunks;

    int ret;

    MirrorOp *op;

    int max_sectors;



    sectors_per_chunk = s->granularity >> BDRV_SECTOR_BITS;

    max_sectors = sectors_per_chunk * s->max_iov;



    

    nb_sectors = MIN(s->buf_size >> BDRV_SECTOR_BITS, nb_sectors);

    nb_sectors = MIN(max_sectors, nb_sectors);

    assert(nb_sectors);

    ret = nb_sectors;



    if (s->cow_bitmap) {

        ret += mirror_cow_align(s, &sector_num, &nb_sectors);

    }

    assert(nb_sectors << BDRV_SECTOR_BITS <= s->buf_size);

    

    assert(!(sector_num % sectors_per_chunk));

    nb_chunks = DIV_ROUND_UP(nb_sectors, sectors_per_chunk);



    while (s->buf_free_count < nb_chunks) {

        trace_mirror_yield_in_flight(s, sector_num * BDRV_SECTOR_SIZE,

                                     s->in_flight);

        mirror_wait_for_io(s);

    }



    

    op = g_new(MirrorOp, 1);

    op->s = s;

    op->sector_num = sector_num;

    op->nb_sectors = nb_sectors;



    

    qemu_iovec_init(&op->qiov, nb_chunks);

    while (nb_chunks-- > 0) {

        MirrorBuffer *buf = QSIMPLEQ_FIRST(&s->buf_free);

        size_t remaining = nb_sectors * BDRV_SECTOR_SIZE - op->qiov.size;



        QSIMPLEQ_REMOVE_HEAD(&s->buf_free, next);

        s->buf_free_count--;

        qemu_iovec_add(&op->qiov, buf, MIN(s->granularity, remaining));

    }



    

    s->in_flight++;

    s->sectors_in_flight += nb_sectors;

    trace_mirror_one_iteration(s, sector_num * BDRV_SECTOR_SIZE,

                               nb_sectors * BDRV_SECTOR_SIZE);



    blk_aio_preadv(source, sector_num * BDRV_SECTOR_SIZE, &op->qiov, 0,

                   mirror_read_complete, op);

    return ret;

}