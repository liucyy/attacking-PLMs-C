static int posix_aio_init(void)

{

    sigset_t mask;

    PosixAioState *s;

  

    if (posix_aio_state)

        return 0;



    s = qemu_malloc(sizeof(PosixAioState));

    if (s == NULL)

        return -ENOMEM;



    

    sigemptyset(&mask);

    sigaddset(&mask, SIGUSR2);

    sigprocmask(SIG_BLOCK, &mask, NULL);

    

    s->first_aio = NULL;

    s->fd = qemu_signalfd(&mask);

    if (s->fd == -1) {

        fprintf(stderr, "failed to create signalfd\n");

        return -errno;

    }



    fcntl(s->fd, F_SETFL, O_NONBLOCK);



    qemu_aio_set_fd_handler(s->fd, posix_aio_read, NULL, posix_aio_flush, s);



#if defined(__linux__)

    {

        struct aioinit ai;



        memset(&ai, 0, sizeof(ai));

#if defined(__GLIBC_PREREQ) && __GLIBC_PREREQ(2, 4)

        ai.aio_threads = 64;

        ai.aio_num = 64;

#else

        

        ai.aio_threads = 1;

        ai.aio_num = 1;

        ai.aio_idle_time = 365 * 100000;

#endif

        aio_init(&ai);

    }

#endif

    posix_aio_state = s;



    return 0;

}