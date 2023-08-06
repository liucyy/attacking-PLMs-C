int64_t qmp_guest_fsfreeze_thaw(Error **err)

{

    int ret;

    GuestFsfreezeMountList mounts;

    GuestFsfreezeMount *mount;

    int fd, i = 0, logged;



    QTAILQ_INIT(&mounts);

    ret = guest_fsfreeze_build_mount_list(&mounts);

    if (ret) {

        error_set(err, QERR_QGA_COMMAND_FAILED,

                  "failed to enumerate filesystems");

        return 0;

    }



    QTAILQ_FOREACH(mount, &mounts, next) {

        logged = false;

        fd = qemu_open(mount->dirname, O_RDONLY);

        if (fd == -1) {

            continue;

        }

        

        do {

            ret = ioctl(fd, FITHAW);

            if (ret == 0 && !logged) {

                i++;

                logged = true;

            }

        } while (ret == 0);

        close(fd);

    }



    guest_fsfreeze_state.status = GUEST_FSFREEZE_STATUS_THAWED;

    enable_logging();

    guest_fsfreeze_free_mount_list(&mounts);

    return i;

}