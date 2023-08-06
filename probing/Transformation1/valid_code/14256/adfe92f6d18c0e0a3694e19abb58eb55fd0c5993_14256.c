static void posix_aio_read(void *opaque)

{

    PosixAioState *s = opaque;

    ssize_t len;



    

    for (;;) {

        char bytes[16];



        len = read(s->rfd, bytes, sizeof(bytes));

        if (len == -1 && errno == EINTR)

            continue; 

        if (len == sizeof(bytes))

            continue; 

        break;

    }



    posix_aio_process_queue(s);

}