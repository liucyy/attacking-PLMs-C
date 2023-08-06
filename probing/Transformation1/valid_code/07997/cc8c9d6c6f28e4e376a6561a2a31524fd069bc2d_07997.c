static void coroutine_fn mirror_iteration(MirrorBlockJob *s)

{

    BlockDriverState *source = s->common.bs;

    int nb_sectors, sectors_per_chunk, nb_chunks;

    int64_t end, sector_num, next_chunk, next_sector, hbitmap_next_sector;

    MirrorOp *op;



    s->sector_num = hbitmap_iter_next(&s->hbi);

    if (s->sector_num < 0) {

        bdrv_dirty_iter_init(source, s->dirty_bitmap, &s->hbi);

        s->sector_num = hbitmap_iter_next(&s->hbi);

        trace_mirror_restart_iter(s,

                                  bdrv_get_dirty_count(source, s->dirty_bitmap));

        assert(s->sector_num >= 0);

    }



    hbitmap_next_sector = s->sector_num;

    sector_num = s->sector_num;

    sectors_per_chunk = s->granularity >> BDRV_SECTOR_BITS;

    end = s->common.len >> BDRV_SECTOR_BITS;



    

    nb_chunks = 0;

    nb_sectors = 0;

    next_sector = sector_num;

    next_chunk = sector_num / sectors_per_chunk;



    

    while (test_bit(next_chunk, s->in_flight_bitmap)) {

        trace_mirror_yield_in_flight(s, sector_num, s->in_flight);

        qemu_coroutine_yield();

    }



    do {

        int added_sectors, added_chunks;



        if (!bdrv_get_dirty(source, s->dirty_bitmap, next_sector) ||

            test_bit(next_chunk, s->in_flight_bitmap)) {

            assert(nb_sectors > 0);

            break;

        }



        added_sectors = sectors_per_chunk;

        if (s->cow_bitmap && !test_bit(next_chunk, s->cow_bitmap)) {

            bdrv_round_to_clusters(s->target,

                                   next_sector, added_sectors,

                                   &next_sector, &added_sectors);



            

            if (next_sector < sector_num) {

                assert(nb_sectors == 0);

                sector_num = next_sector;

                next_chunk = next_sector / sectors_per_chunk;

            }

        }



        added_sectors = MIN(added_sectors, end - (sector_num + nb_sectors));

        added_chunks = (added_sectors + sectors_per_chunk - 1) / sectors_per_chunk;



        

        while (nb_chunks == 0 && s->buf_free_count < added_chunks) {

            trace_mirror_yield_buf_busy(s, nb_chunks, s->in_flight);

            qemu_coroutine_yield();

        }

        if (s->buf_free_count < nb_chunks + added_chunks) {

            trace_mirror_break_buf_busy(s, nb_chunks, s->in_flight);

            break;

        }



        

        bitmap_set(s->in_flight_bitmap, next_chunk, added_chunks);



        nb_sectors += added_sectors;

        nb_chunks += added_chunks;

        next_sector += added_sectors;

        next_chunk += added_chunks;

    } while (next_sector < end);



    

    op = g_slice_new(MirrorOp);

    op->s = s;

    op->sector_num = sector_num;

    op->nb_sectors = nb_sectors;



    

    qemu_iovec_init(&op->qiov, nb_chunks);

    next_sector = sector_num;

    while (nb_chunks-- > 0) {

        MirrorBuffer *buf = QSIMPLEQ_FIRST(&s->buf_free);

        QSIMPLEQ_REMOVE_HEAD(&s->buf_free, next);

        s->buf_free_count--;

        qemu_iovec_add(&op->qiov, buf, s->granularity);



        

        if (next_sector > hbitmap_next_sector

            && bdrv_get_dirty(source, s->dirty_bitmap, next_sector)) {

            hbitmap_next_sector = hbitmap_iter_next(&s->hbi);

        }



        next_sector += sectors_per_chunk;

    }



    bdrv_reset_dirty(source, sector_num, nb_sectors);



    

    s->in_flight++;

    trace_mirror_one_iteration(s, sector_num, nb_sectors);

    bdrv_aio_readv(source, sector_num, &op->qiov, nb_sectors,

                   mirror_read_complete, op);

}