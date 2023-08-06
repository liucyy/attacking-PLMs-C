void bdrv_delete(BlockDriverState *bs)

{

    assert(!bs->peer);



    

    bdrv_make_anon(bs);



    bdrv_close(bs);

    if (bs->file != NULL) {

        bdrv_delete(bs->file);

    }



    assert(bs != bs_snapshots);

    g_free(bs);

}