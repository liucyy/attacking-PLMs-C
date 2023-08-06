void bdrv_set_aio_context(BlockDriverState *bs, AioContext *new_context)
{
    bdrv_drain(bs); 
    bdrv_detach_aio_context(bs);
    
    aio_context_acquire(new_context);
    bdrv_attach_aio_context(bs, new_context);
    aio_context_release(new_context);