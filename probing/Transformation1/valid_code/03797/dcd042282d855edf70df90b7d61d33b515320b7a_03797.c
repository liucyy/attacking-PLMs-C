AioContext *bdrv_get_aio_context(BlockDriverState *bs)

{

    

    return qemu_get_aio_context();

}