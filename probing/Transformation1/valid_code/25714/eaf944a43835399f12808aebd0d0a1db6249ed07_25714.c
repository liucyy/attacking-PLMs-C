static int blkdebug_open(BlockDriverState *bs, QDict *options, int flags,

                         Error **errp)

{

    BDRVBlkdebugState *s = bs->opaque;

    QemuOpts *opts;

    Error *local_err = NULL;

    const char *config;

    uint64_t align;

    int ret;



    opts = qemu_opts_create(&runtime_opts, NULL, 0, &error_abort);

    qemu_opts_absorb_qdict(opts, options, &local_err);

    if (error_is_set(&local_err)) {

        error_propagate(errp, local_err);

        ret = -EINVAL;

        goto fail;

    }



    

    config = qemu_opt_get(opts, "config");

    ret = read_config(s, config, options, errp);

    if (ret) {

        goto fail;

    }



    

    s->state = 1;



    

    ret = bdrv_open_image(&bs->file, qemu_opt_get(opts, "x-image"), options, "image",

                          flags, true, false, &local_err);

    if (ret < 0) {

        error_propagate(errp, local_err);

        goto fail;

    }



    

    align = qemu_opt_get_size(opts, "align", bs->request_alignment);

    if (align > 0 && align < INT_MAX && !(align & (align - 1))) {

        bs->request_alignment = align;

    } else {

        error_setg(errp, "Invalid alignment");

        ret = -EINVAL;

        goto fail;

    }



    ret = 0;

fail:

    qemu_opts_del(opts);

    return ret;

}