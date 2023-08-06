long do_sigreturn(CPUSPARCState *env)

{

    abi_ulong sf_addr;

    struct target_signal_frame *sf;

    uint32_t up_psr, pc, npc;

    target_sigset_t set;

    sigset_t host_set;

    int err=0, i;



    sf_addr = env->regwptr[UREG_FP];

    trace_user_do_sigreturn(env, sf_addr);

    if (!lock_user_struct(VERIFY_READ, sf, sf_addr, 1)) {

        goto segv_and_exit;

    }



    



    if (sf_addr & 3)

        goto segv_and_exit;



    __get_user(pc,  &sf->info.si_regs.pc);

    __get_user(npc, &sf->info.si_regs.npc);



    if ((pc | npc) & 3) {

        goto segv_and_exit;

    }



    

    __get_user(up_psr, &sf->info.si_regs.psr);



    

    env->psr = (up_psr & (PSR_ICC ))

            | (env->psr & ~(PSR_ICC ));



    env->pc = pc;

    env->npc = npc;

    __get_user(env->y, &sf->info.si_regs.y);

    for (i=0; i < 8; i++) {

        __get_user(env->gregs[i], &sf->info.si_regs.u_regs[i]);

    }

    for (i=0; i < 8; i++) {

        __get_user(env->regwptr[i + UREG_I0], &sf->info.si_regs.u_regs[i+8]);

    }



    



    

    __get_user(set.sig[0], &sf->info.si_mask);

    for(i = 1; i < TARGET_NSIG_WORDS; i++) {

        __get_user(set.sig[i], &sf->extramask[i - 1]);

    }



    target_to_host_sigset_internal(&host_set, &set);

    do_sigprocmask(SIG_SETMASK, &host_set, NULL);



    if (err) {

        goto segv_and_exit;

    }

    unlock_user_struct(sf, sf_addr, 0);

    return env->regwptr[0];



segv_and_exit:

    unlock_user_struct(sf, sf_addr, 0);

    force_sig(TARGET_SIGSEGV);

}