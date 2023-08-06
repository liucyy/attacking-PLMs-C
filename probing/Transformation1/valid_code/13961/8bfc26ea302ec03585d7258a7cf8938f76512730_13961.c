uint32_t HELPER(v7m_mrs)(CPUARMState *env, uint32_t reg)

{

    uint32_t mask;

    unsigned el = arm_current_el(env);



    



    switch (reg) {

    case 0 ... 7: 

        mask = 0;

        if ((reg & 1) && el) {

            mask |= XPSR_EXCP; 

        }

        if (!(reg & 4)) {

            mask |= XPSR_NZCV | XPSR_Q; 

        }

        

        return xpsr_read(env) & mask;

        break;

    case 20: 

        return env->v7m.control;

    }



    if (el == 0) {

        return 0; 

    }



    switch (reg) {

    case 8: 

        return (env->v7m.control & R_V7M_CONTROL_SPSEL_MASK) ?

            env->v7m.other_sp : env->regs[13];

    case 9: 

        return (env->v7m.control & R_V7M_CONTROL_SPSEL_MASK) ?

            env->regs[13] : env->v7m.other_sp;

    case 16: 

        return env->v7m.primask[env->v7m.secure];

    case 17: 

    case 18: 

        return env->v7m.basepri[env->v7m.secure];

    case 19: 

        return env->v7m.faultmask[env->v7m.secure];

    default:

        qemu_log_mask(LOG_GUEST_ERROR, "Attempt to read unknown special"

                                       " register %d\n", reg);

        return 0;

    }

}