static int local_remove(FsContext *ctx, const char *path)

{

    int err;

    struct stat stbuf;

    char buffer[PATH_MAX];



    if (ctx->export_flags & V9FS_SM_MAPPED_FILE) {

        err =  lstat(rpath(ctx, path, buffer), &stbuf);

        if (err) {

            goto err_out;

        }

        

        if (S_ISDIR(stbuf.st_mode)) {

            snprintf(buffer, ARRAY_SIZE(buffer), "%s/%s/%s",

                     ctx->fs_root, path, VIRTFS_META_DIR);

            err = remove(buffer);

            if (err < 0 && errno != ENOENT) {

                

                goto err_out;

            }

        }

        

        err = remove(local_mapped_attr_path(ctx, path, buffer));

        if (err < 0 && errno != ENOENT) {

            

            goto err_out;

        }

    }

    return remove(rpath(ctx, path, buffer));

err_out:

    return err;

}