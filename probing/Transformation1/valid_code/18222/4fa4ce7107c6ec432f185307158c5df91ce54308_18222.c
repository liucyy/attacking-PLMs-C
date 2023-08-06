static int local_unlinkat(FsContext *ctx, V9fsPath *dir,

                          const char *name, int flags)

{

    int ret;

    V9fsString fullname;

    char buffer[PATH_MAX];



    v9fs_string_init(&fullname);



    v9fs_string_sprintf(&fullname, "%s/%s", dir->data, name);

    if (ctx->export_flags & V9FS_SM_MAPPED_FILE) {

        if (flags == AT_REMOVEDIR) {

            

            snprintf(buffer, ARRAY_SIZE(buffer), "%s/%s/%s", ctx->fs_root,

                     fullname.data, VIRTFS_META_DIR);

            ret = remove(buffer);

            if (ret < 0 && errno != ENOENT) {

                

                goto err_out;

            }

        }

        

        ret = remove(local_mapped_attr_path(ctx, fullname.data, buffer));

        if (ret < 0 && errno != ENOENT) {

            

            goto err_out;

        }

    }

    

    ret = remove(rpath(ctx, fullname.data, buffer));



err_out:

    v9fs_string_free(&fullname);

    return ret;

}