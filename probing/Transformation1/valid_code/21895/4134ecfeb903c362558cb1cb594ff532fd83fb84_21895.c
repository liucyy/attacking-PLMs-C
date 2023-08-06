long do_sigreturn(CPUMBState *env)

{

    struct target_signal_frame *frame;

    abi_ulong frame_addr;

    target_sigset_t target_set;

    sigset_t set;

    int i;



    frame_addr = env->regs[R_SP];

    trace_user_do_sigreturn(env, frame_addr);

    

    if (!lock_user_struct(VERIFY_WRITE, frame, frame_addr, 1))

        goto badframe;



    

    __get_user(target_set.sig[0], &frame->uc.tuc_mcontext.oldmask);

    for(i = 1; i < TARGET_NSIG_WORDS; i++) {

        __get_user(target_set.sig[i], &frame->extramask[i - 1]);

    }

    target_to_host_sigset_internal(&set, &target_set);

    do_sigprocmask(SIG_SETMASK, &set, NULL);



    restore_sigcontext(&frame->uc.tuc_mcontext, env);

    

    env->regs[14] = env->sregs[SR_PC];



    unlock_user_struct(frame, frame_addr, 0);

    return env->regs[10];

badframe:

    force_sig(TARGET_SIGSEGV);

}