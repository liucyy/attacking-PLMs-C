static int proxy_ioc_getversion(FsContext *fs_ctx, V9fsPath *path,

                                mode_t st_mode, uint64_t *st_gen)

{

    int err;



    

    if (!S_ISREG(st_mode) && !S_ISDIR(st_mode)) {

        errno = ENOTTY;

        return -1;

    }

    err = v9fs_request(fs_ctx->private, T_GETVERSION, st_gen, "s", path);

    if (err < 0) {

        errno = -err;

        err = -1;

    }

    return err;

}