void do_interrupt(CPUARMState *env)

{

    uint32_t addr;

    uint32_t mask;

    int new_mode;

    uint32_t offset;



    if (IS_M(env)) {

        do_interrupt_v7m(env);

        return;

    }

    

    switch (env->exception_index) {

    case EXCP_UDEF:

        new_mode = ARM_CPU_MODE_UND;

        addr = 0x04;

        mask = CPSR_I;

        if (env->thumb)

            offset = 2;

        else

            offset = 4;

        break;

    case EXCP_SWI:

        if (semihosting_enabled) {

            

            if (env->thumb) {

                mask = lduw_code(env->regs[15] - 2) & 0xff;

            } else {

                mask = ldl_code(env->regs[15] - 4) & 0xffffff;

            }

            

            if (((mask == 0x123456 && !env->thumb)

                    || (mask == 0xab && env->thumb))

                  && (env->uncached_cpsr & CPSR_M) != ARM_CPU_MODE_USR) {

                env->regs[0] = do_arm_semihosting(env);

                return;

            }

        }

        new_mode = ARM_CPU_MODE_SVC;

        addr = 0x08;

        mask = CPSR_I;

        

        offset = 0;

        break;

    case EXCP_BKPT:

        

        if (env->thumb && semihosting_enabled) {

            mask = lduw_code(env->regs[15]) & 0xff;

            if (mask == 0xab

                  && (env->uncached_cpsr & CPSR_M) != ARM_CPU_MODE_USR) {

                env->regs[15] += 2;

                env->regs[0] = do_arm_semihosting(env);

                return;

            }

        }

        

    case EXCP_PREFETCH_ABORT:

        new_mode = ARM_CPU_MODE_ABT;

        addr = 0x0c;

        mask = CPSR_A | CPSR_I;

        offset = 4;

        break;

    case EXCP_DATA_ABORT:

        new_mode = ARM_CPU_MODE_ABT;

        addr = 0x10;

        mask = CPSR_A | CPSR_I;

        offset = 8;

        break;

    case EXCP_IRQ:

        new_mode = ARM_CPU_MODE_IRQ;

        addr = 0x18;

        

        mask = CPSR_A | CPSR_I;

        offset = 4;

        break;

    case EXCP_FIQ:

        new_mode = ARM_CPU_MODE_FIQ;

        addr = 0x1c;

        

        mask = CPSR_A | CPSR_I | CPSR_F;

        offset = 4;

        break;

    default:

        cpu_abort(env, "Unhandled exception 0x%x\n", env->exception_index);

        return; 

    }

    

    if (env->cp15.c1_sys & (1 << 13)) {

        addr += 0xffff0000;

    }

    switch_mode (env, new_mode);

    env->spsr = cpsr_read(env);

    

    env->condexec_bits = 0;

    

    env->uncached_cpsr = (env->uncached_cpsr & ~CPSR_M) | new_mode;

    env->uncached_cpsr |= mask;

    env->thumb = (env->cp15.c1_sys & (1 << 30)) != 0;

    env->regs[14] = env->regs[15] + offset;

    env->regs[15] = addr;

    env->interrupt_request |= CPU_INTERRUPT_EXITTB;

}