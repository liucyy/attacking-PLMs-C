int pt_removexattr(FsContext *ctx, const char *path, const char *name)

{

    char *buffer;

    int ret;



    buffer = rpath(ctx, path);

    ret = lremovexattr(path, name);

    g_free(buffer);

    return ret;

}