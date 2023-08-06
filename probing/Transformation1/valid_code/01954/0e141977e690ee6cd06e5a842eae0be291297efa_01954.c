void cpu_loop(CPUAlphaState *env)

{

    int trapnr;

    target_siginfo_t info;

    abi_long sysret;



    while (1) {

        trapnr = cpu_alpha_exec (env);



        

        env->intr_flag = 0;



        switch (trapnr) {

        case EXCP_RESET:

            fprintf(stderr, "Reset requested. Exit\n");

            exit(1);

            break;

        case EXCP_MCHK:

            fprintf(stderr, "Machine check exception. Exit\n");

            exit(1);

            break;

        case EXCP_SMP_INTERRUPT:

        case EXCP_CLK_INTERRUPT:

        case EXCP_DEV_INTERRUPT:

            fprintf(stderr, "External interrupt. Exit\n");

            exit(1);

            break;

        case EXCP_MMFAULT:

            env->lock_addr = -1;

            info.si_signo = TARGET_SIGSEGV;

            info.si_errno = 0;

            info.si_code = (page_get_flags(env->trap_arg0) & PAGE_VALID

                            ? TARGET_SEGV_ACCERR : TARGET_SEGV_MAPERR);

            info._sifields._sigfault._addr = env->trap_arg0;

            queue_signal(env, info.si_signo, &info);

            break;

        case EXCP_UNALIGN:

            env->lock_addr = -1;

            info.si_signo = TARGET_SIGBUS;

            info.si_errno = 0;

            info.si_code = TARGET_BUS_ADRALN;

            info._sifields._sigfault._addr = env->trap_arg0;

            queue_signal(env, info.si_signo, &info);

            break;

        case EXCP_OPCDEC:

        do_sigill:

            env->lock_addr = -1;

            info.si_signo = TARGET_SIGILL;

            info.si_errno = 0;

            info.si_code = TARGET_ILL_ILLOPC;

            info._sifields._sigfault._addr = env->pc;

            queue_signal(env, info.si_signo, &info);

            break;

        case EXCP_ARITH:

            env->lock_addr = -1;

            info.si_signo = TARGET_SIGFPE;

            info.si_errno = 0;

            info.si_code = TARGET_FPE_FLTINV;

            info._sifields._sigfault._addr = env->pc;

            queue_signal(env, info.si_signo, &info);

            break;

        case EXCP_FEN:

            

            break;

        case EXCP_CALL_PAL:

            env->lock_addr = -1;

            switch (env->error_code) {

            case 0x80:

                

                info.si_signo = TARGET_SIGTRAP;

                info.si_errno = 0;

                info.si_code = TARGET_TRAP_BRKPT;

                info._sifields._sigfault._addr = env->pc;

                queue_signal(env, info.si_signo, &info);

                break;

            case 0x81:

                

                info.si_signo = TARGET_SIGTRAP;

                info.si_errno = 0;

                info.si_code = 0;

                info._sifields._sigfault._addr = env->pc;

                queue_signal(env, info.si_signo, &info);

                break;

            case 0x83:

                

                trapnr = env->ir[IR_V0];

                sysret = do_syscall(env, trapnr,

                                    env->ir[IR_A0], env->ir[IR_A1],

                                    env->ir[IR_A2], env->ir[IR_A3],

                                    env->ir[IR_A4], env->ir[IR_A5],

                                    0, 0);

                if (trapnr == TARGET_NR_sigreturn

                    || trapnr == TARGET_NR_rt_sigreturn) {

                    break;

                }

                

                if (env->ir[IR_V0] == 0) {

                    env->ir[IR_V0] = sysret;

                } else {

                    env->ir[IR_V0] = (sysret < 0 ? -sysret : sysret);

                    env->ir[IR_A3] = (sysret < 0);

                }

                break;

            case 0x86:

                

                

                break;

            case 0x9E:

                

                

                abort();

            case 0x9F:

                

                

                abort();

            case 0xAA:

                

                info.si_signo = TARGET_SIGFPE;

                switch (env->ir[IR_A0]) {

                case TARGET_GEN_INTOVF:

                    info.si_code = TARGET_FPE_INTOVF;

                    break;

                case TARGET_GEN_INTDIV:

                    info.si_code = TARGET_FPE_INTDIV;

                    break;

                case TARGET_GEN_FLTOVF:

                    info.si_code = TARGET_FPE_FLTOVF;

                    break;

                case TARGET_GEN_FLTUND:

                    info.si_code = TARGET_FPE_FLTUND;

                    break;

                case TARGET_GEN_FLTINV:

                    info.si_code = TARGET_FPE_FLTINV;

                    break;

                case TARGET_GEN_FLTINE:

                    info.si_code = TARGET_FPE_FLTRES;

                    break;

                case TARGET_GEN_ROPRAND:

                    info.si_code = 0;

                    break;

                default:

                    info.si_signo = TARGET_SIGTRAP;

                    info.si_code = 0;

                    break;

                }

                info.si_errno = 0;

                info._sifields._sigfault._addr = env->pc;

                queue_signal(env, info.si_signo, &info);

                break;

            default:

                goto do_sigill;

            }

            break;

        case EXCP_DEBUG:

            info.si_signo = gdb_handlesig (env, TARGET_SIGTRAP);

            if (info.si_signo) {

                env->lock_addr = -1;

                info.si_errno = 0;

                info.si_code = TARGET_TRAP_BRKPT;

                queue_signal(env, info.si_signo, &info);

            }

            break;

        case EXCP_STL_C:

        case EXCP_STQ_C:

            do_store_exclusive(env, env->error_code, trapnr - EXCP_STL_C);

            break;

        case EXCP_INTERRUPT:

            

            break;

        default:

            printf ("Unhandled trap: 0x%x\n", trapnr);

            cpu_dump_state(env, stderr, fprintf, 0);

            exit (1);

        }

        process_pending_signals (env);

    }

}