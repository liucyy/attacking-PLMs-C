static inline int handle_cpu_signal(uintptr_t pc, siginfo_t *info,

                                    int is_write, sigset_t *old_set)

{

    CPUState *cpu = current_cpu;

    CPUClass *cc;

    int ret;

    unsigned long address = (unsigned long)info->si_addr;



    

    if (helper_retaddr) {

        pc = helper_retaddr;

    } else {

        pc += GETPC_ADJ;

    }



    

    if (!cpu || !cpu->running) {

        printf("qemu:%s received signal outside vCPU context @ pc=0x%"

               PRIxPTR "\n",  __func__, pc);

        abort();

    }



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

            

            helper_retaddr = 0;

            cpu_exit_tb_from_sighandler(cpu, old_set);

            



        default:

            g_assert_not_reached();

        }

    }



    

    address = h2g_nocheck(address);



    cc = CPU_GET_CLASS(cpu);

    

    g_assert(cc->handle_mmu_fault);

    ret = cc->handle_mmu_fault(cpu, address, is_write, MMU_USER_IDX);



    if (ret == 0) {

        

        return 1;

    }



    

    helper_retaddr = 0;



    if (ret < 0) {

        return 0; 

    }



    

    cpu_restore_state(cpu, pc);



    sigprocmask(SIG_SETMASK, old_set, NULL);

    cpu_loop_exit(cpu);



    

    return 1;

}