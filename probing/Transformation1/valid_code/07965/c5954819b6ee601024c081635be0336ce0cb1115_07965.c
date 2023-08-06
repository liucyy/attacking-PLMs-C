static inline int handle_cpu_signal(uintptr_t pc, unsigned long address,

                                    int is_write, sigset_t *old_set,

                                    void *puc)

{

    TranslationBlock *tb;

    int ret;



    if (cpu_single_env) {

        env = cpu_single_env; 

    }

#if defined(DEBUG_SIGNAL)

    qemu_printf("qemu: SIGSEGV pc=0x%08lx address=%08lx w=%d oldset=0x%08lx\n",

                pc, address, is_write, *(unsigned long *)old_set);

#endif

    

    if (is_write && page_unprotect(h2g(address), pc, puc)) {

        return 1;

    }



    

    ret = cpu_handle_mmu_fault(env, address, is_write, MMU_USER_IDX);

    if (ret < 0) {

        return 0; 

    }

    if (ret == 0) {

        return 1; 

    }

    

    tb = tb_find_pc(pc);

    if (tb) {

        

        cpu_restore_state(tb, env, pc);

    }



    

    sigprocmask(SIG_SETMASK, old_set, NULL);

    exception_action(env);



    

    return 1;

}