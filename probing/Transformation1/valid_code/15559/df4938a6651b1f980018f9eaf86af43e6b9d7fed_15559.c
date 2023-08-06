static int local_unlinkat(FsContext *ctx, V9fsPath *dir,

                          const char *name, int flags)

{

    int ret;

    V9fsString fullname;

    char *buffer;



    v9fs_string_init(&fullname);



    v9fs_string_sprintf(&fullname, "%s/%s", dir->data, name);

    if (ctx->export_flags & V9FS_SM_MAPPED_FILE) {

        if (flags == AT_REMOVEDIR) {

            

            buffer = g_strdup_printf("%s/%s/%s", ctx->fs_root,

                                     fullname.data, VIRTFS_META_DIR);

            ret = remove(buffer);

            g_free(buffer);

            if (ret < 0 && errno != ENOENT) {

                

                goto err_out;

            }

        }

        

        buffer = local_mapped_attr_path(ctx, fullname.data);

        ret = remove(buffer);

        g_free(buffer);

        if (ret < 0 && errno != ENOENT) {

            

            goto err_out;

        }

    }

    

    buffer = rpath(ctx, fullname.data);

    ret = remove(buffer);

    g_free(buffer);



err_out:

    v9fs_string_free(&fullname);

    return ret;

}