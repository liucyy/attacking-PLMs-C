static int mp_user_removexattr(FsContext *ctx,

                               const char *path, const char *name)

{

    char *buffer;

    int ret;



    if (strncmp(name, "user.virtfs.", 12) == 0) {

        

        errno = EACCES;

        return -1;

    }

    buffer = rpath(ctx, path);

    ret = lremovexattr(buffer, name);

    g_free(buffer);

    return ret;

}