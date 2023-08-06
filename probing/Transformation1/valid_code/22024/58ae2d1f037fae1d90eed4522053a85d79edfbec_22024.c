static int bad_mode_switch(CPUARMState *env, int mode)

{

    

    switch (mode) {

    case ARM_CPU_MODE_USR:

    case ARM_CPU_MODE_SYS:

    case ARM_CPU_MODE_SVC:

    case ARM_CPU_MODE_ABT:

    case ARM_CPU_MODE_UND:

    case ARM_CPU_MODE_IRQ:

    case ARM_CPU_MODE_FIQ:

        

        return 0;

    case ARM_CPU_MODE_HYP:

        return !arm_feature(env, ARM_FEATURE_EL2)

            || arm_current_el(env) < 2 || arm_is_secure(env);

    case ARM_CPU_MODE_MON:

        return !arm_is_secure(env);

    default:

        return 1;

    }

}