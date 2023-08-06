void bdrv_append(BlockDriverState *bs_new, BlockDriverState *bs_top)

{

    assert(!bdrv_requests_pending(bs_top));

    assert(!bdrv_requests_pending(bs_new));



    bdrv_ref(bs_top);



    change_parent_backing_link(bs_top, bs_new);

    bdrv_set_backing_hd(bs_new, bs_top);

    bdrv_unref(bs_top);



    

    bdrv_unref(bs_new);

}