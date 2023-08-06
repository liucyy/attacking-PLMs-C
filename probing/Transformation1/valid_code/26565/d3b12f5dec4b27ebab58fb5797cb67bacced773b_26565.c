static void qemu_event_read(void *opaque)

{

    int fd = (intptr_t)opaque;

    ssize_t len;

    char buffer[512];



    

    do {

        len = read(fd, buffer, sizeof(buffer));

    } while ((len == -1 && errno == EINTR) || len == sizeof(buffer));

}