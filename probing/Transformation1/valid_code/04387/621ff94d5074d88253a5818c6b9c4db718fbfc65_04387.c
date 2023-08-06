void qmp_change_backing_file(const char *device,

                             const char *image_node_name,

                             const char *backing_file,

                             Error **errp)

{

    BlockBackend *blk;

    BlockDriverState *bs = NULL;

    AioContext *aio_context;

    BlockDriverState *image_bs = NULL;

    Error *local_err = NULL;

    bool ro;

    int open_flags;

    int ret;



    blk = blk_by_name(device);

    if (!blk) {

        error_set(errp, ERROR_CLASS_DEVICE_NOT_FOUND,

                  "Device '%s' not found", device);

        return;

    }



    aio_context = blk_get_aio_context(blk);

    aio_context_acquire(aio_context);



    if (!blk_is_available(blk)) {

        error_setg(errp, "Device '%s' has no medium", device);

        goto out;

    }

    bs = blk_bs(blk);



    image_bs = bdrv_lookup_bs(NULL, image_node_name, &local_err);

    if (local_err) {

        error_propagate(errp, local_err);

        goto out;

    }



    if (!image_bs) {

        error_setg(errp, "image file not found");

        goto out;

    }



    if (bdrv_find_base(image_bs) == image_bs) {

        error_setg(errp, "not allowing backing file change on an image "

                         "without a backing file");

        goto out;

    }



    

    if (bdrv_op_is_blocked(bs, BLOCK_OP_TYPE_CHANGE, errp)) {

        goto out;

    }



    

    if (!bdrv_chain_contains(bs, image_bs)) {

        error_setg(errp, "'%s' and image file are not in the same chain",

                   device);

        goto out;

    }



    

    open_flags = image_bs->open_flags;

    ro = bdrv_is_read_only(image_bs);



    if (ro) {

        bdrv_reopen(image_bs, open_flags | BDRV_O_RDWR, &local_err);

        if (local_err) {

            error_propagate(errp, local_err);

            goto out;

        }

    }



    ret = bdrv_change_backing_file(image_bs, backing_file,

                               image_bs->drv ? image_bs->drv->format_name : "");



    if (ret < 0) {

        error_setg_errno(errp, -ret, "Could not change backing file to '%s'",

                         backing_file);

        

    }



    if (ro) {

        bdrv_reopen(image_bs, open_flags, &local_err);

        if (local_err) {

            error_propagate(errp, local_err); 

        }

    }



out:

    aio_context_release(aio_context);

}