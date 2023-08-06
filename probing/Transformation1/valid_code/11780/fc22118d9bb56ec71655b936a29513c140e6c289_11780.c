static ssize_t local_lgetxattr(FsContext *ctx, const char *path,

                               const char *name, void *value, size_t size)

{

    if ((ctx->fs_sm == SM_MAPPED) &&

        (strncmp(name, "user.virtfs.", 12) == 0)) {

        

        errno = ENOATTR;

        return -1;

    }



    return lgetxattr(rpath(ctx, path), name, value, size);

}