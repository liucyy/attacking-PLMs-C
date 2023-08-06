static BlockAIOCB *blkverify_aio_flush(BlockDriverState *bs,

                                       BlockCompletionFunc *cb,

                                       void *opaque)

{

    BDRVBlkverifyState *s = bs->opaque;



    

    return bdrv_aio_flush(s->test_file->bs, cb, opaque);

}