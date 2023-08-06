static int get_cpsr(QEMUFile *f, void *opaque, size_t size,

                    VMStateField *field)

{

    ARMCPU *cpu = opaque;

    CPUARMState *env = &cpu->env;

    uint32_t val = qemu_get_be32(f);



    if (arm_feature(env, ARM_FEATURE_M)) {

        if (val & XPSR_EXCP) {

            

            uint32_t newval = val;



            newval &= (CPSR_NZCV | CPSR_Q | CPSR_IT | CPSR_GE);

            if (val & CPSR_T) {

                newval |= XPSR_T;

            }

            

            if (val & CPSR_F) {

                env->v7m.faultmask = 1;

            }

            if (val & CPSR_I) {

                env->v7m.primask = 1;

            }

            val = newval;

        }

        

        xpsr_write(env, val, ~XPSR_EXCP);

        return 0;

    }



    env->aarch64 = ((val & PSTATE_nRW) == 0);



    if (is_a64(env)) {

        pstate_write(env, val);

        return 0;

    }



    cpsr_write(env, val, 0xffffffff, CPSRWriteRaw);

    return 0;

}