static int mp_dacl_removexattr(FsContext *ctx,

                               const char *path, const char *name)

{

    int ret;

    char buffer[PATH_MAX];

    ret  = lremovexattr(rpath(ctx, path, buffer), MAP_ACL_DEFAULT);

    if (ret == -1 && errno == ENODATA) {

        

        errno = 0;

        ret = 0;

    }

    return ret;

}