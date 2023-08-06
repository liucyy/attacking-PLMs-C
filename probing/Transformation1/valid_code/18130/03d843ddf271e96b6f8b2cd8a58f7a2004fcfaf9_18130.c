static inline int handle_cpu_signal(unsigned long pc,

                                    unsigned long address,

                                    int is_write,

                                    sigset_t *old_set)

{

#if defined(DEBUG_SIGNAL)

    printf("qemu: SIGSEGV pc=0x%08lx address=%08lx wr=%d oldset=0x%08lx\n", 

           pc, address, is_write, *(unsigned long *)old_set);

#endif

    

    if (is_write && page_unprotect(address)) {

        sigprocmask(SIG_SETMASK, old_set, NULL);

        return 1;

    }

    if (pc >= (unsigned long)code_gen_buffer &&

        pc < (unsigned long)code_gen_buffer + CODE_GEN_BUFFER_SIZE) {

        

        

        sigprocmask(SIG_SETMASK, old_set, NULL);

        

        env->cr2 = address;

        raise_exception_err(EXCP0E_PAGE, 4 | (is_write << 1));

        

        return 1;

    } else {

        return 0;

    }

}