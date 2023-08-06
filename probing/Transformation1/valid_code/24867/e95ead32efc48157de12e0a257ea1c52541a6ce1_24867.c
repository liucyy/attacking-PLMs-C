static int local_fsync(FsContext *ctx, int fd)

{

    if (0) 

        (void)local_set_xattr(NULL, NULL);

    return fsync(fd);

}