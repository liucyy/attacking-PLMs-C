static int qemu_signal_init(void)

{

    int sigfd;

    sigset_t set;



#ifdef CONFIG_IOTHREAD

    

    sigemptyset(&set);

    sigaddset(&set, SIGUSR2);

    pthread_sigmask(SIG_UNBLOCK, &set, NULL);



    

    sigemptyset(&set);

    sigaddset(&set, SIG_IPI);

    pthread_sigmask(SIG_BLOCK, &set, NULL);



    sigemptyset(&set);

    sigaddset(&set, SIGIO);

    sigaddset(&set, SIGALRM);

    sigaddset(&set, SIGBUS);

#else

    sigemptyset(&set);

    sigaddset(&set, SIGBUS);

    if (kvm_enabled()) {

        

        sigaddset(&set, SIGIO);

        sigaddset(&set, SIGALRM);

    }

#endif

    pthread_sigmask(SIG_BLOCK, &set, NULL);



    sigfd = qemu_signalfd(&set);

    if (sigfd == -1) {

        fprintf(stderr, "failed to create signalfd\n");

        return -errno;

    }



    fcntl_setfl(sigfd, O_NONBLOCK);



    qemu_set_fd_handler2(sigfd, NULL, sigfd_handler, NULL,

                         (void *)(intptr_t)sigfd);



    return 0;

}