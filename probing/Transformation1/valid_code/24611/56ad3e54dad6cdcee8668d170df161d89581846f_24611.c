static ssize_t mp_dacl_getxattr(FsContext *ctx, const char *path,

                                const char *name, void *value, size_t size)

{

    char *buffer;

    ssize_t ret;



    buffer = rpath(ctx, path);

    ret = lgetxattr(buffer, MAP_ACL_DEFAULT, value, size);

    g_free(buffer);

    return ret;

}