get_sigframe(struct emulated_sigaction *ka, CPUX86State *env, size_t frame_size)

{

	unsigned long esp;



	

	esp = env->regs[R_ESP];

#if 0

	

	if (ka->sa.sa_flags & SA_ONSTACK) {

		if (sas_ss_flags(esp) == 0)

			esp = current->sas_ss_sp + current->sas_ss_size;

	}



	

	else if ((regs->xss & 0xffff) != __USER_DS &&

		 !(ka->sa.sa_flags & SA_RESTORER) &&

		 ka->sa.sa_restorer) {

		esp = (unsigned long) ka->sa.sa_restorer;

	}

#endif

	return (void *)((esp - frame_size) & -8ul);

}