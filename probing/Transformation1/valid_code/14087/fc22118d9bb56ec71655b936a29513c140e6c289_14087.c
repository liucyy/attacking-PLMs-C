static int local_lsetxattr(FsContext *ctx, const char *path, const char *name,

                           void *value, size_t size, int flags)

{

    if ((ctx->fs_sm == SM_MAPPED) &&

        (strncmp(name, "user.virtfs.", 12) == 0)) {

        

        errno = EACCES;

        return -1;

    }

    return lsetxattr(rpath(ctx, path), name, value, size, flags);

}