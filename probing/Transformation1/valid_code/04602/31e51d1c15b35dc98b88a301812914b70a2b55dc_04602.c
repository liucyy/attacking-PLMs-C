static int local_statfs(FsContext *s, V9fsPath *fs_path, struct statfs *stbuf)

{

    char *buffer;

    int ret;

    char *path = fs_path->data;



    buffer = rpath(s, path);

    ret = statfs(buffer, stbuf);

    g_free(buffer);

    return ret;

}