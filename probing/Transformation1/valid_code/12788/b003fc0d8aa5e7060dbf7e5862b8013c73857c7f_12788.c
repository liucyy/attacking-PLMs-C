static int local_unlinkat_common(FsContext *ctx, int dirfd, const char *name,

                                 int flags)

{

    int ret = -1;



    if (ctx->export_flags & V9FS_SM_MAPPED_FILE) {

        int map_dirfd;



        if (flags == AT_REMOVEDIR) {

            int fd;



            fd = openat(dirfd, name, O_RDONLY | O_DIRECTORY | O_PATH);

            if (fd == -1) {

                goto err_out;

            }

            

            ret = unlinkat(fd, VIRTFS_META_DIR, AT_REMOVEDIR);

            close_preserve_errno(fd);

            if (ret < 0 && errno != ENOENT) {

                

                goto err_out;

            }

        }

        

        map_dirfd = openat_dir(dirfd, VIRTFS_META_DIR);

        ret = unlinkat(map_dirfd, name, 0);

        close_preserve_errno(map_dirfd);

        if (ret < 0 && errno != ENOENT) {

            

            goto err_out;

        }

    }



    ret = unlinkat(dirfd, name, flags);

err_out:

    return ret;

}