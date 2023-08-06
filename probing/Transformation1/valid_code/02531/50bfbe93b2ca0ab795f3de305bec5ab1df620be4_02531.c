BlockBackend *blk_new_open(const char *filename, const char *reference,

                           QDict *options, int flags, Error **errp)

{

    BlockBackend *blk;

    BlockDriverState *bs;

    uint64_t perm;



    

    perm = BLK_PERM_CONSISTENT_READ;

    if (flags & BDRV_O_RDWR) {

        perm |= BLK_PERM_WRITE;

    }

    if (flags & BDRV_O_RESIZE) {

        perm |= BLK_PERM_RESIZE;

    }



    blk = blk_new(perm, BLK_PERM_ALL);

    bs = bdrv_open(filename, reference, options, flags, errp);

    if (!bs) {

        blk_unref(blk);

        return NULL;

    }



    blk->root = bdrv_root_attach_child(bs, "root", &child_root,

                                       perm, BLK_PERM_ALL, blk, &error_abort);



    return blk;

}