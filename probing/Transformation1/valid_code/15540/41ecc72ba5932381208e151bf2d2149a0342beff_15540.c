static void setup_rt_frame(int sig, struct target_sigaction *ka,

                           target_siginfo_t *info,

                           target_sigset_t *set, CPUOpenRISCState *env)

{

    int err = 0;

    abi_ulong frame_addr;

    unsigned long return_ip;

    struct target_rt_sigframe *frame;

    abi_ulong info_addr, uc_addr;



    frame_addr = get_sigframe(ka, env, sizeof(*frame));

    if (!lock_user_struct(VERIFY_WRITE, frame, frame_addr, 0)) {

        goto give_sigsegv;

    }



    info_addr = frame_addr + offsetof(struct target_rt_sigframe, info);

    __put_user(info_addr, &frame->pinfo);

    uc_addr = frame_addr + offsetof(struct target_rt_sigframe, uc);

    __put_user(uc_addr, &frame->puc);



    if (ka->sa_flags & SA_SIGINFO) {

        copy_siginfo_to_user(&frame->info, info);

    }



    

    __put_user(0, &frame->uc.tuc_flags);

    __put_user(0, &frame->uc.tuc_link);

    __put_user(target_sigaltstack_used.ss_sp,

               &frame->uc.tuc_stack.ss_sp);

    __put_user(sas_ss_flags(env->gpr[1]), &frame->uc.tuc_stack.ss_flags);

    __put_user(target_sigaltstack_used.ss_size,

               &frame->uc.tuc_stack.ss_size);

    err |= setup_sigcontext(&frame->sc, env, set->sig[0]);



    



    if (err) {

        goto give_sigsegv;

    }



    

    return_ip = (unsigned long)&frame->retcode;

    

    __put_user(0xa960, (short *)(frame->retcode + 0));

    __put_user(TARGET_NR_rt_sigreturn, (short *)(frame->retcode + 2));

    __put_user(0x20000001, (unsigned long *)(frame->retcode + 4));

    __put_user(0x15000000, (unsigned long *)(frame->retcode + 8));



    if (err) {

        goto give_sigsegv;

    }



    



    

    env->pc = (unsigned long)ka->_sa_handler; 

    env->gpr[9] = (unsigned long)return_ip;     

    env->gpr[3] = (unsigned long)sig;           

    env->gpr[4] = (unsigned long)&frame->info;  

    env->gpr[5] = (unsigned long)&frame->uc;    



    

    env->gpr[1] = (unsigned long)frame;



    return;



give_sigsegv:

    unlock_user_struct(frame, frame_addr, 1);

    if (sig == TARGET_SIGSEGV) {

        ka->_sa_handler = TARGET_SIG_DFL;

    }

    force_sig(TARGET_SIGSEGV);

}