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

                apsrmask |= 0xf8000000; 

            }

            if ((mask & 4) && arm_feature(env, ARM_FEATURE_THUMB_DSP)) {

                apsrmask |= 0x000f0000; 

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

        if (val & 1) {

            env->daif |= PSTATE_I;

        } else {

            env->daif &= ~PSTATE_I;

        }

        break;

    case 17: 

        env->v7m.basepri = val & 0xff;

        break;

    case 18: 

        val &= 0xff;

        if (val != 0 && (val < env->v7m.basepri || env->v7m.basepri == 0))

            env->v7m.basepri = val;

        break;

    case 19: 

        if (val & 1) {

            env->daif |= PSTATE_F;

        } else {

            env->daif &= ~PSTATE_F;

        }

        break;

    case 20: 

        switch_v7m_sp(env, (val & R_V7M_CONTROL_SPSEL_MASK) != 0);

        env->v7m.control = val & (R_V7M_CONTROL_SPSEL_MASK |

                                  R_V7M_CONTROL_NPRIV_MASK);

        break;

    default:

        qemu_log_mask(LOG_GUEST_ERROR, "Attempt to write unknown special"

                                       " register %d\n", reg);

        return;

    }

}