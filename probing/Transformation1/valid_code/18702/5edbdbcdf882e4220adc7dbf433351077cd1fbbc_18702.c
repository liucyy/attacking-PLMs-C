static int check_shm_size(IVShmemState *s, int fd) {

    



    struct stat buf;



    fstat(fd, &buf);



    if (s->ivshmem_size > buf.st_size) {

        fprintf(stderr,

                "IVSHMEM ERROR: Requested memory size greater"

                " than shared object size (%" PRIu64 " > %" PRIu64")\n",

                s->ivshmem_size, (uint64_t)buf.st_size);

        return -1;

    } else {

        return 0;

    }

}