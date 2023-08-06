int cpu_exec(CPUArchState *env)

{

    CPUState *cpu = ENV_GET_CPU(env);

    CPUClass *cc = CPU_GET_CLASS(cpu);

#ifdef TARGET_I386

    X86CPU *x86_cpu = X86_CPU(cpu);

#endif

    int ret, interrupt_request;

    TranslationBlock *tb;

    uint8_t *tc_ptr;

    uintptr_t next_tb;

    SyncClocks sc;



    

    volatile bool have_tb_lock = false;



    if (cpu->halted) {

        if (!cpu_has_work(cpu)) {

            return EXCP_HALTED;

        }



        cpu->halted = 0;

    }



    current_cpu = cpu;



    

    smp_mb();



    if (unlikely(exit_request)) {

        cpu->exit_request = 1;

    }



    cc->cpu_exec_enter(cpu);

    cpu->exception_index = -1;



    

    init_delay_params(&sc, cpu);



    

    for(;;) {

        if (sigsetjmp(cpu->jmp_env, 0) == 0) {

            

            if (cpu->exception_index >= 0) {

                if (cpu->exception_index >= EXCP_INTERRUPT) {

                    

                    ret = cpu->exception_index;

                    if (ret == EXCP_DEBUG) {

                        cpu_handle_debug_exception(env);

                    }

                    break;

                } else {

#if defined(CONFIG_USER_ONLY)

                    

#if defined(TARGET_I386)

                    cc->do_interrupt(cpu);

#endif

                    ret = cpu->exception_index;

                    break;

#else

                    cc->do_interrupt(cpu);

                    cpu->exception_index = -1;

#endif

                }

            }



            next_tb = 0; 

            for(;;) {

                interrupt_request = cpu->interrupt_request;

                if (unlikely(interrupt_request)) {

                    if (unlikely(cpu->singlestep_enabled & SSTEP_NOIRQ)) {

                        

                        interrupt_request &= ~CPU_INTERRUPT_SSTEP_MASK;

                    }

                    if (interrupt_request & CPU_INTERRUPT_DEBUG) {

                        cpu->interrupt_request &= ~CPU_INTERRUPT_DEBUG;

                        cpu->exception_index = EXCP_DEBUG;

                        cpu_loop_exit(cpu);

                    }

                    if (interrupt_request & CPU_INTERRUPT_HALT) {

                        cpu->interrupt_request &= ~CPU_INTERRUPT_HALT;

                        cpu->halted = 1;

                        cpu->exception_index = EXCP_HLT;

                        cpu_loop_exit(cpu);

                    }

#if defined(TARGET_I386)

                    if (interrupt_request & CPU_INTERRUPT_INIT) {

                        cpu_svm_check_intercept_param(env, SVM_EXIT_INIT, 0);

                        do_cpu_init(x86_cpu);

                        cpu->exception_index = EXCP_HALTED;

                        cpu_loop_exit(cpu);

                    }

#else

                    if (interrupt_request & CPU_INTERRUPT_RESET) {

                        cpu_reset(cpu);

                    }

#endif

                    

                    if (cc->cpu_exec_interrupt(cpu, interrupt_request)) {

                        next_tb = 0;

                    }

                    

                    if (cpu->interrupt_request & CPU_INTERRUPT_EXITTB) {

                        cpu->interrupt_request &= ~CPU_INTERRUPT_EXITTB;

                        

                        next_tb = 0;

                    }

                }

                if (unlikely(cpu->exit_request)) {

                    cpu->exit_request = 0;

                    cpu->exception_index = EXCP_INTERRUPT;

                    cpu_loop_exit(cpu);

                }

                spin_lock(&tcg_ctx.tb_ctx.tb_lock);

                have_tb_lock = true;

                tb = tb_find_fast(env);

                

                if (tcg_ctx.tb_ctx.tb_invalidated_flag) {

                    

                    next_tb = 0;

                    tcg_ctx.tb_ctx.tb_invalidated_flag = 0;

                }

                if (qemu_loglevel_mask(CPU_LOG_EXEC)) {

                    qemu_log("Trace %p [" TARGET_FMT_lx "] %s\n",

                             tb->tc_ptr, tb->pc, lookup_symbol(tb->pc));

                }

                

                if (next_tb != 0 && tb->page_addr[1] == -1) {

                    tb_add_jump((TranslationBlock *)(next_tb & ~TB_EXIT_MASK),

                                next_tb & TB_EXIT_MASK, tb);

                }

                have_tb_lock = false;

                spin_unlock(&tcg_ctx.tb_ctx.tb_lock);



                

                cpu->current_tb = tb;

                barrier();

                if (likely(!cpu->exit_request)) {

                    trace_exec_tb(tb, tb->pc);

                    tc_ptr = tb->tc_ptr;

                    

                    next_tb = cpu_tb_exec(cpu, tc_ptr);

                    switch (next_tb & TB_EXIT_MASK) {

                    case TB_EXIT_REQUESTED:

                        

                        tb = (TranslationBlock *)(next_tb & ~TB_EXIT_MASK);

                        next_tb = 0;

                        break;

                    case TB_EXIT_ICOUNT_EXPIRED:

                    {

                        

                        int insns_left;

                        tb = (TranslationBlock *)(next_tb & ~TB_EXIT_MASK);

                        insns_left = cpu->icount_decr.u32;

                        if (cpu->icount_extra && insns_left >= 0) {

                            

                            cpu->icount_extra += insns_left;

                            if (cpu->icount_extra > 0xffff) {

                                insns_left = 0xffff;

                            } else {

                                insns_left = cpu->icount_extra;

                            }

                            cpu->icount_extra -= insns_left;

                            cpu->icount_decr.u16.low = insns_left;

                        } else {

                            if (insns_left > 0) {

                                

                                cpu_exec_nocache(env, insns_left, tb);

                                align_clocks(&sc, cpu);

                            }

                            cpu->exception_index = EXCP_INTERRUPT;

                            next_tb = 0;

                            cpu_loop_exit(cpu);

                        }

                        break;

                    }

                    default:

                        break;

                    }

                }

                cpu->current_tb = NULL;

                

                align_clocks(&sc, cpu);

                

            } 

        } else {

            

            cpu = current_cpu;

            env = cpu->env_ptr;

            cc = CPU_GET_CLASS(cpu);

#ifdef TARGET_I386

            x86_cpu = X86_CPU(cpu);

#endif

            if (have_tb_lock) {

                spin_unlock(&tcg_ctx.tb_ctx.tb_lock);

                have_tb_lock = false;

            }

        }

    } 



    cc->cpu_exec_exit(cpu);



    

    current_cpu = NULL;

    return ret;

}