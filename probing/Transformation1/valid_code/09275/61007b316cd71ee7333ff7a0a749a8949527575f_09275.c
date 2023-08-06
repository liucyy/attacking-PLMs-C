int bdrv_pwrite_sync(BlockDriverState *bs, int64_t offset,

    const void *buf, int count)

{

    int ret;



    ret = bdrv_pwrite(bs, offset, buf, count);

    if (ret < 0) {

        return ret;

    }



    

    if (bs->enable_write_cache) {

        bdrv_flush(bs);

    }



    return 0;

}