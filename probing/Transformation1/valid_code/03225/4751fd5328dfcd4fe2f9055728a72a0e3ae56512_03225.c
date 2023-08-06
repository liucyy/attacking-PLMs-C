static int fchmodat_nofollow(int dirfd, const char *name, mode_t mode)

{

    int fd, ret;



    

    fd = openat_file(dirfd, name, O_RDONLY, 0);

    if (fd == -1) {

        

        if (errno == EACCES) {

            fd = openat_file(dirfd, name, O_WRONLY, 0);

        }

        if (fd == -1 && errno == EISDIR) {

            errno = EACCES;

        }

    }

    if (fd == -1) {

        return -1;

    }

    ret = fchmod(fd, mode);

    close_preserve_errno(fd);

    return ret;

}