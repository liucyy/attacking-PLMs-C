static int mp_user_removexattr(FsContext *ctx,

                               const char *path, const char *name)

{

    char buffer[PATH_MAX];

    if (strncmp(name, "user.virtfs.", 12) == 0) {

        

        errno = EACCES;

        return -1;

    }

    return lremovexattr(rpath(ctx, path, buffer), name);

}