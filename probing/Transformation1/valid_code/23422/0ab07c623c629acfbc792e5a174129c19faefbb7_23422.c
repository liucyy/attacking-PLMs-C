static void qemu_event_increment(void)

{

    

    static const uint64_t val = 1;

    ssize_t ret;



    if (io_thread_fd == -1)

        return;



    do {

        ret = write(io_thread_fd, &val, sizeof(val));

    } while (ret < 0 && errno == EINTR);



    

    if (ret < 0 && errno != EAGAIN) {

        fprintf(stderr, "qemu_event_increment: write() filed: %s\n",

                strerror(errno));

        exit (1);

    }

}