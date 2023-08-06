static void vmdk_close(BlockDriverState *bs)

{

    BDRVVmdkState *s = bs->opaque;



    qemu_free(s->l1_table);

    qemu_free(s->l2_cache);

    bdrv_delete(s->hd);

    

    vmdk_parent_close(s->hd);

}