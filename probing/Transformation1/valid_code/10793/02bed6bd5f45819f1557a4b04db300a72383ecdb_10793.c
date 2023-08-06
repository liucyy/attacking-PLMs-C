static inline int handle_cpu_signal(uintptr_t pc, unsigned long address,

                                    int is_write, sigset_t *old_set)

{

    CPUState *cpu;

    CPUClass *cc;

    int ret;



#if defined(DEBUG_SIGNAL)

    printf("qemu: SIGSEGV pc=0x%08lx address=%08lx w=%d oldset=0x%08lx\n",

           pc, address, is_write, *(unsigned long *)old_set);

#endif

    

    if (is_write && h2g_valid(address)) {

        switch (page_unprotect(h2g(address), pc)) {

        case 0:

            

            break;

        case 1:

            

            return 1;

        case 2:

            

            cpu_exit_tb_from_sighandler(current_cpu, old_set);

            g_assert_not_reached();

        default:

            g_assert_not_reached();

        }

    }



    

    address = h2g_nocheck(address);



    cpu = current_cpu;

    cc = CPU_GET_CLASS(cpu);

    

    g_assert(cc->handle_mmu_fault);

    ret = cc->handle_mmu_fault(cpu, address, is_write, MMU_USER_IDX);

    if (ret < 0) {

        return 0; 

    }

    if (ret == 0) {

        return 1; 

    }



    

    cpu_restore_state(cpu, pc + GETPC_ADJ);



    sigprocmask(SIG_SETMASK, old_set, NULL);

    cpu_loop_exit(cpu);



    

    return 1;

}