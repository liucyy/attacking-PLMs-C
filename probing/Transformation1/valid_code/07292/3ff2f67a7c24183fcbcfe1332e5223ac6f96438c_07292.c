int coroutine_fn bdrv_co_flush(BlockDriverState *bs)
{
    int ret;
    BdrvTrackedRequest req;
    if (!bs || !bdrv_is_inserted(bs) || bdrv_is_read_only(bs) ||
        bdrv_is_sg(bs)) {
        return 0;
    }
    tracked_request_begin(&req, bs, 0, 0, BDRV_TRACKED_FLUSH);
    int current_gen = bs->write_gen;
    
    while (bs->flush_started_gen != bs->flushed_gen) {
        qemu_co_queue_wait(&bs->flush_queue);
    }
    bs->flush_started_gen = current_gen;
    
    if (bs->drv->bdrv_co_flush) {
        ret = bs->drv->bdrv_co_flush(bs);
        goto out;
    }
    
    BLKDBG_EVENT(bs->file, BLKDBG_FLUSH_TO_OS);
    if (bs->drv->bdrv_co_flush_to_os) {
        ret = bs->drv->bdrv_co_flush_to_os(bs);
        if (ret < 0) {
            goto out;
        }
    }
    
    if (bs->open_flags & BDRV_O_NO_FLUSH) {
        goto flush_parent;
    }
    
    if (bs->flushed_gen == current_gen) {
        goto flush_parent;
    }
    BLKDBG_EVENT(bs->file, BLKDBG_FLUSH_TO_DISK);
    if (bs->drv->bdrv_co_flush_to_disk) {
        ret = bs->drv->bdrv_co_flush_to_disk(bs);
    } else if (bs->drv->bdrv_aio_flush) {
        BlockAIOCB *acb;
        CoroutineIOCompletion co = {
            .coroutine = qemu_coroutine_self(),
        };
        acb = bs->drv->bdrv_aio_flush(bs, bdrv_co_io_em_complete, &co);
        if (acb == NULL) {
            ret = -EIO;
        } else {
            qemu_coroutine_yield();
            ret = co.ret;
        }
    } else {
        
        ret = 0;
    }
    if (ret < 0) {
        goto out;
    }
    
flush_parent:
    ret = bs->file ? bdrv_co_flush(bs->file->bs) : 0;
out:
    
    bs->flushed_gen = current_gen;
    qemu_co_queue_restart_all(&bs->flush_queue);
    tracked_request_end(&req);
    return ret;
}