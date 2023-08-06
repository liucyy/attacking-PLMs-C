static int local_post_create_passthrough(FsContext *fs_ctx, const char *path,

                                         FsCred *credp)

{

    char buffer[PATH_MAX];



    if (chmod(rpath(fs_ctx, path, buffer), credp->fc_mode & 07777) < 0) {

        return -1;

    }

    if (lchown(rpath(fs_ctx, path, buffer), credp->fc_uid,

                credp->fc_gid) < 0) {

        

        if (fs_ctx->fs_sm != SM_NONE) {

            return -1;

        }

    }

    return 0;

}