long do_rt_sigreturn(CPUState *env)

{

	struct rt_sigframe *frame;

        sigset_t host_set;



	

	if (env->regs[13] & 7)

		goto badframe;



	frame = (struct rt_sigframe *)env->regs[13];



#if 0

	if (verify_area(VERIFY_READ, frame, sizeof (*frame)))

		goto badframe;

#endif

        target_to_host_sigset(&host_set, &frame->uc.tuc_sigmask);

        sigprocmask(SIG_SETMASK, &host_set, NULL);



	if (restore_sigcontext(env, &frame->uc.tuc_mcontext))

		goto badframe;



	if (do_sigaltstack(h2g(&frame->uc.tuc_stack), 0, get_sp_from_cpustate(env)) == -EFAULT)

		goto badframe;



#if 0

	

	if (ptrace_cancel_bpt(current))

		send_sig(SIGTRAP, current, 1);

#endif

	return env->regs[0];



badframe:

        force_sig(SIGSEGV );

	return 0;

}