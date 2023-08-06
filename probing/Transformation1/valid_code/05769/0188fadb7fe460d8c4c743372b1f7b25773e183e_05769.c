static void setup_frame(int sig, struct target_sigaction *ka,

                        target_sigset_t *set, CPUS390XState *env)

{

    sigframe *frame;

    abi_ulong frame_addr;



    frame_addr = get_sigframe(ka, env, sizeof(*frame));

    qemu_log("%s: frame_addr 0x%llx\n", __FUNCTION__,

             (unsigned long long)frame_addr);

    if (!lock_user_struct(VERIFY_WRITE, frame, frame_addr, 0)) {

            goto give_sigsegv;

    }



    qemu_log("%s: 1\n", __FUNCTION__);

    if (__put_user(set->sig[0], &frame->sc.oldmask[0])) {

              goto give_sigsegv;

    }



    save_sigregs(env, &frame->sregs);



    __put_user((abi_ulong)(unsigned long)&frame->sregs,

               (abi_ulong *)&frame->sc.sregs);



    

    if (ka->sa_flags & TARGET_SA_RESTORER) {

            env->regs[14] = (unsigned long)

                    ka->sa_restorer | PSW_ADDR_AMODE;

    } else {

            env->regs[14] = (unsigned long)

                    frame->retcode | PSW_ADDR_AMODE;

            if (__put_user(S390_SYSCALL_OPCODE | TARGET_NR_sigreturn,

                           (uint16_t *)(frame->retcode)))

                    goto give_sigsegv;

    }



    

    if (__put_user(env->regs[15], (abi_ulong *) frame)) {

            goto give_sigsegv;

    }



    

    env->regs[15] = frame_addr;

    env->psw.addr = (target_ulong) ka->_sa_handler | PSW_ADDR_AMODE;



    env->regs[2] = sig; 

    env->regs[3] = frame_addr += offsetof(typeof(*frame), sc);



    

    env->regs[4] = 0; 

    env->regs[5] = 0; 



    

    if (__put_user(env->regs[2], (int *) &frame->signo)) {

            goto give_sigsegv;

    }

    unlock_user_struct(frame, frame_addr, 1);

    return;



give_sigsegv:

    qemu_log("%s: give_sigsegv\n", __FUNCTION__);

    unlock_user_struct(frame, frame_addr, 1);

    force_sig(TARGET_SIGSEGV);

}