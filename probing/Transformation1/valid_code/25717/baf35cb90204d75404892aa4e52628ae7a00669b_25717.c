void qemu_aio_init(void)

{

    struct sigaction act;



    aio_initialized = 1;



    sigfillset(&act.sa_mask);

    act.sa_flags = 0; 

    act.sa_handler = aio_signal_handler;

    sigaction(aio_sig_num, &act, NULL);



#if defined(__GLIBC__) && defined(__linux__)

    {

        

        struct aioinit ai;

        memset(&ai, 0, sizeof(ai));

        ai.aio_threads = 1;

        ai.aio_num = 1;

        ai.aio_idle_time = 365 * 100000;

        aio_init(&ai);

    }

#endif

}