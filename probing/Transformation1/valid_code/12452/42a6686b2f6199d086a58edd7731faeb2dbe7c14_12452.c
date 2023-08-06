void HELPER(v7m_msr)(CPUARMState *env, uint32_t maskreg, uint32_t val)

{

    

    uint32_t mask = extract32(maskreg, 8, 4);

    uint32_t reg = extract32(maskreg, 0, 8);



    if (arm_current_el(env) == 0 && reg > 7) {

        

        return;

    }



    switch (reg) {

    case 0 ... 7: 

        

        if (!(reg & 4)) {

            uint32_t apsrmask = 0;



            if (mask & 8) {

                apsrmask |= XPSR_NZCV | XPSR_Q;

            }

            if ((mask & 4) && arm_feature(env, ARM_FEATURE_THUMB_DSP)) {

                apsrmask |= XPSR_GE;

            }

            xpsr_write(env, val, apsrmask);

        }

        break;

    case 8: 

        if (env->v7m.control & R_V7M_CONTROL_SPSEL_MASK) {

            env->v7m.other_sp = val;

        } else {

            env->regs[13] = val;

        }

        break;

    case 9: 

        if (env->v7m.control & R_V7M_CONTROL_SPSEL_MASK) {

            env->regs[13] = val;

        } else {

            env->v7m.other_sp = val;

        }

        break;

    case 16: 

        env->v7m.primask[env->v7m.secure] = val & 1;

        break;

    case 17: 

        env->v7m.basepri[env->v7m.secure] = val & 0xff;

        break;

    case 18: 

        val &= 0xff;

        if (val != 0 && (val < env->v7m.basepri[env->v7m.secure]

                         || env->v7m.basepri[env->v7m.secure] == 0)) {

            env->v7m.basepri[env->v7m.secure] = val;

        }

        break;

    case 19: 

        env->v7m.faultmask = val & 1;

        break;

    case 20: 

        

        if (!arm_v7m_is_handler_mode(env)) {

            switch_v7m_sp(env, (val & R_V7M_CONTROL_SPSEL_MASK) != 0);

        }

        env->v7m.control &= ~R_V7M_CONTROL_NPRIV_MASK;

        env->v7m.control |= val & R_V7M_CONTROL_NPRIV_MASK;

        break;

    default:

        qemu_log_mask(LOG_GUEST_ERROR, "Attempt to write unknown special"

                                       " register %d\n", reg);

        return;

    }

}