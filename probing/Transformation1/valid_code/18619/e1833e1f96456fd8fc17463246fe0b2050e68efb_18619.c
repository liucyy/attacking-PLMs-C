void ppc_hw_interrupt (CPUPPCState *env)

{

    int raised = 0;



#if 1

    if (loglevel & CPU_LOG_INT) {

        fprintf(logfile, "%s: %p pending %08x req %08x me %d ee %d\n",

                __func__, env, env->pending_interrupts,

                env->interrupt_request, msr_me, msr_ee);

    }

#endif

    

    if (env->pending_interrupts & (1 << PPC_INTERRUPT_RESET)) {

        

        

        env->exception_index = EXCP_RESET;

        env->pending_interrupts &= ~(1 << PPC_INTERRUPT_RESET);

        raised = 1;

    }

    if (raised == 0 && msr_me != 0) {

        

        if (env->pending_interrupts & (1 << PPC_INTERRUPT_MCK)) {

            env->exception_index = EXCP_MACHINE_CHECK;

            env->pending_interrupts &= ~(1 << PPC_INTERRUPT_MCK);

            raised = 1;

        }

    }

    if (raised == 0 && msr_ee != 0) {

#if defined(TARGET_PPC64H) 

        

        if (env->pending_interrupts & (1 << PPC_INTERRUPT_HDECR)) {

            env->exception_index = EXCP_HDECR;

            env->pending_interrupts &= ~(1 << PPC_INTERRUPT_HDECR);

            raised = 1;

        } else

#endif

        

        if (env->pending_interrupts & (1 << PPC_INTERRUPT_DECR)) {

            env->exception_index = EXCP_DECR;

            env->pending_interrupts &= ~(1 << PPC_INTERRUPT_DECR);

            raised = 1;

        

        } else if (env->pending_interrupts & (1 << PPC_INTERRUPT_PIT)) {

            env->exception_index = EXCP_40x_PIT;

            env->pending_interrupts &= ~(1 << PPC_INTERRUPT_PIT);

            raised = 1;

        

        } else if (env->pending_interrupts & (1 << PPC_INTERRUPT_FIT)) {

            env->exception_index = EXCP_40x_FIT;

            env->pending_interrupts &= ~(1 << PPC_INTERRUPT_FIT);

            raised = 1;

        

        } else if (env->pending_interrupts & (1 << PPC_INTERRUPT_WDT)) {

            env->exception_index = EXCP_40x_WATCHDOG;

            env->pending_interrupts &= ~(1 << PPC_INTERRUPT_WDT);

            raised = 1;

        

        } else if (env->pending_interrupts & (1 << PPC_INTERRUPT_EXT)) {

            env->exception_index = EXCP_EXTERNAL;

            

#if 0

            env->pending_interrupts &= ~(1 << PPC_INTERRUPT_EXT);

#endif

            raised = 1;

#if 0 

        

        } else if (env->pending_interrupts & (1 << PPC_INTERRUPT_THERM)) {

            env->exception_index = EXCP_970_THRM;

            env->pending_interrupts &= ~(1 << PPC_INTERRUPT_THERM);

            raised = 1;

#endif

        }

#if 0 

    

    } else if (env->pending_interrupts & (1 << PPC_INTERRUPT_DEBUG)) {

        env->exception_index = EXCP_xxx;

        env->pending_interrupts &= ~(1 << PPC_INTERRUPT_DEBUG);

        raised = 1;

#endif

    }

    if (raised != 0) {

        env->error_code = 0;

        do_interrupt(env);

    }

}