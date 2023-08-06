static inline int handle_cpu_signal(unsigned long pc, unsigned long address,

                                    int is_write, sigset_t *old_set, 

                                    void *puc)

{

    TranslationBlock *tb;

    int ret;



    if (cpu_single_env)

        env = cpu_single_env; 

#if defined(DEBUG_SIGNAL)

    qemu_printf("qemu: SIGSEGV pc=0x%08lx address=%08lx w=%d oldset=0x%08lx\n", 

                pc, address, is_write, *(unsigned long *)old_set);

#endif

    

    if (is_write && page_unprotect(address, pc, puc)) {

        return 1;

    }



    

    ret = cpu_x86_handle_mmu_fault(env, address, is_write, 

                                   ((env->hflags & HF_CPL_MASK) == 3), 0);

    if (ret < 0)

        return 0; 

    if (ret == 0)

        return 1; 

    

    tb = tb_find_pc(pc);

    if (tb) {

        

        cpu_restore_state(tb, env, pc, puc);

    }

    if (ret == 1) {

#if 0

        printf("PF exception: EIP=0x%08x CR2=0x%08x error=0x%x\n", 

               env->eip, env->cr[2], env->error_code);

#endif

        

        sigprocmask(SIG_SETMASK, old_set, NULL);

        raise_exception_err(EXCP0E_PAGE, env->error_code);

    } else {

        

        env->hflags |= HF_SOFTMMU_MASK;

        cpu_resume_from_signal(env, puc);

    }

    

    return 1;

}