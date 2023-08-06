static void setup_frame(int sig, struct target_sigaction * ka,

                        target_sigset_t *set, CPUMIPSState *regs)

{

    struct sigframe *frame;

    abi_ulong frame_addr;

    int i;



    frame_addr = get_sigframe(ka, regs, sizeof(*frame));

    if (!lock_user_struct(VERIFY_WRITE, frame, frame_addr, 0))

	goto give_sigsegv;



    install_sigtramp(frame->sf_code, TARGET_NR_sigreturn);



    setup_sigcontext(regs, &frame->sf_sc);



    for(i = 0; i < TARGET_NSIG_WORDS; i++) {

	if(__put_user(set->sig[i], &frame->sf_mask.sig[i]))

	    goto give_sigsegv;

    }



    

    regs->active_tc.gpr[ 4] = sig;

    regs->active_tc.gpr[ 5] = 0;

    regs->active_tc.gpr[ 6] = frame_addr + offsetof(struct sigframe, sf_sc);

    regs->active_tc.gpr[29] = frame_addr;

    regs->active_tc.gpr[31] = frame_addr + offsetof(struct sigframe, sf_code);

    

    regs->active_tc.PC = regs->active_tc.gpr[25] = ka->_sa_handler;

    mips_set_hflags_isa_mode_from_pc(regs);

    unlock_user_struct(frame, frame_addr, 1);

    return;



give_sigsegv:

    unlock_user_struct(frame, frame_addr, 1);

    force_sig(TARGET_SIGSEGV);

}