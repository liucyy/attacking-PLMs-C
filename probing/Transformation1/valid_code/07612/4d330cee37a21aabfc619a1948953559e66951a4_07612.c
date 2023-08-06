static void host_signal_handler(int host_signum, siginfo_t *info,
                                void *puc)
{
    CPUArchState *env = thread_cpu->env_ptr;
    int sig;
    target_siginfo_t tinfo;
    
    if ((host_signum == SIGSEGV || host_signum == SIGBUS)
        && info->si_code > 0) {
        if (cpu_signal_handler(host_signum, info, puc))
            return;
    }
    
    sig = host_to_target_signal(host_signum);
    if (sig < 1 || sig > TARGET_NSIG)
        return;
    trace_user_host_signal(env, host_signum, sig);
    host_to_target_siginfo_noswap(&tinfo, info);
    if (queue_signal(env, sig, &tinfo) == 1) {
        
        cpu_exit(thread_cpu);
    }
}