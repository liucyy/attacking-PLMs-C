static bool qemu_gluster_test_seek(struct glfs_fd *fd)

{

    off_t ret, eof;



    eof = glfs_lseek(fd, 0, SEEK_END);

    if (eof < 0) {

        

        return false;

    }



    

    ret = glfs_lseek(fd, eof, SEEK_DATA);

    return (ret < 0) && (errno == ENXIO);

}