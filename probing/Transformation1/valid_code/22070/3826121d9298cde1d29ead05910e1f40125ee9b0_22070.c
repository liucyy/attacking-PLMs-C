void arm_debug_excp_handler(CPUState *cs)

{

    

    ARMCPU *cpu = ARM_CPU(cs);

    CPUARMState *env = &cpu->env;

    CPUWatchpoint *wp_hit = cs->watchpoint_hit;



    if (wp_hit) {

        if (wp_hit->flags & BP_CPU) {

            cs->watchpoint_hit = NULL;

            if (check_watchpoints(cpu)) {

                bool wnr = (wp_hit->flags & BP_WATCHPOINT_HIT_WRITE) != 0;

                bool same_el = arm_debug_target_el(env) == arm_current_el(env);



                if (extended_addresses_enabled(env)) {

                    env->exception.fsr = (1 << 9) | 0x22;

                } else {

                    env->exception.fsr = 0x2;

                }

                env->exception.vaddress = wp_hit->hitaddr;

                raise_exception(env, EXCP_DATA_ABORT,

                                syn_watchpoint(same_el, 0, wnr),

                                arm_debug_target_el(env));

            } else {

                cpu_resume_from_signal(cs, NULL);

            }

        }

    } else {

        uint64_t pc = is_a64(env) ? env->pc : env->regs[15];

        bool same_el = (arm_debug_target_el(env) == arm_current_el(env));



        

        if (cpu_breakpoint_test(cs, pc, BP_GDB)

            || !cpu_breakpoint_test(cs, pc, BP_CPU)) {

            return;

        }



        if (extended_addresses_enabled(env)) {

            env->exception.fsr = (1 << 9) | 0x22;

        } else {

            env->exception.fsr = 0x2;

        }

        

        raise_exception(env, EXCP_PREFETCH_ABORT,

                        syn_breakpoint(same_el),

                        arm_debug_target_el(env));

    }

}