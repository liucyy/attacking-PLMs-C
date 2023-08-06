void qmp_guest_fstrim(bool has_minimum, int64_t minimum, Error **errp)

{

    int ret = 0;

    FsMountList mounts;

    struct FsMount *mount;

    int fd;

    Error *local_err = NULL;

    struct fstrim_range r = {

        .start = 0,

        .len = -1,

        .minlen = has_minimum ? minimum : 0,

    };



    slog("guest-fstrim called");



    QTAILQ_INIT(&mounts);

    build_fs_mount_list(&mounts, &local_err);

    if (local_err) {

        error_propagate(errp, local_err);

        return;

    }



    QTAILQ_FOREACH(mount, &mounts, next) {

        fd = qemu_open(mount->dirname, O_RDONLY);

        if (fd == -1) {

            error_setg_errno(errp, errno, "failed to open %s", mount->dirname);

            goto error;

        }



        

        ret = ioctl(fd, FITRIM, &r);

        if (ret == -1) {

            if (errno != ENOTTY && errno != EOPNOTSUPP) {

                error_setg_errno(errp, errno, "failed to trim %s",

                                 mount->dirname);

                close(fd);

                goto error;

            }

        }

        close(fd);

    }



error:

    free_fs_mount_list(&mounts);

}