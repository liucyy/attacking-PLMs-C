int block_signals(void)

{

    TaskState *ts = (TaskState *)thread_cpu->opaque;

    sigset_t set;

    int pending;



    

    sigfillset(&set);

    sigprocmask(SIG_SETMASK, &set, 0);



    pending = atomic_xchg(&ts->signal_pending, 1);



    return pending;

}