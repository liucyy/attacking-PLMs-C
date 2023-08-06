static void QEMU_NORETURN force_sig(int target_sig)

{

    CPUState *cpu = thread_cpu;

    CPUArchState *env = cpu->env_ptr;

    TaskState *ts = (TaskState *)cpu->opaque;

    int host_sig, core_dumped = 0;

    struct sigaction act;



    host_sig = target_to_host_signal(target_sig);

    trace_user_force_sig(env, target_sig, host_sig);

    gdb_signalled(env, target_sig);



    

    if (core_dump_signal(target_sig) && (ts->bprm->core_dump != NULL)) {

        stop_all_tasks();

        core_dumped =

            ((*ts->bprm->core_dump)(target_sig, env) == 0);

    }

    if (core_dumped) {

        

        struct rlimit nodump;

        getrlimit(RLIMIT_CORE, &nodump);

        nodump.rlim_cur=0;

        setrlimit(RLIMIT_CORE, &nodump);

        (void) fprintf(stderr, "qemu: uncaught target signal %d (%s) - %s\n",

            target_sig, strsignal(host_sig), "core dumped" );

    }



    

    sigfillset(&act.sa_mask);

    act.sa_handler = SIG_DFL;

    act.sa_flags = 0;

    sigaction(host_sig, &act, NULL);



    

    kill(getpid(), host_sig);



    

    sigdelset(&act.sa_mask, host_sig);

    sigsuspend(&act.sa_mask);



    

    abort();

}