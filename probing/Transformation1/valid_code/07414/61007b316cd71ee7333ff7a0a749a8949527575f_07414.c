int coroutine_fn bdrv_co_flush(BlockDriverState *bs)

{

    int ret;



    if (!bs || !bdrv_is_inserted(bs) || bdrv_is_read_only(bs)) {

        return 0;

    }



    

    BLKDBG_EVENT(bs->file, BLKDBG_FLUSH_TO_OS);

    if (bs->drv->bdrv_co_flush_to_os) {

        ret = bs->drv->bdrv_co_flush_to_os(bs);

        if (ret < 0) {

            return ret;

        }

    }



    

    if (bs->open_flags & BDRV_O_NO_FLUSH) {

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

        return ret;

    }



    

flush_parent:

    return bdrv_co_flush(bs->file);

}