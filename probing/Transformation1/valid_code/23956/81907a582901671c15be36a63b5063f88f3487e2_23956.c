void cpsr_write(CPUARMState *env, uint32_t val, uint32_t mask,

                CPSRWriteType write_type)

{

    uint32_t changed_daif;



    if (mask & CPSR_NZCV) {

        env->ZF = (~val) & CPSR_Z;

        env->NF = val;

        env->CF = (val >> 29) & 1;

        env->VF = (val << 3) & 0x80000000;

    }

    if (mask & CPSR_Q)

        env->QF = ((val & CPSR_Q) != 0);

    if (mask & CPSR_T)

        env->thumb = ((val & CPSR_T) != 0);

    if (mask & CPSR_IT_0_1) {

        env->condexec_bits &= ~3;

        env->condexec_bits |= (val >> 25) & 3;

    }

    if (mask & CPSR_IT_2_7) {

        env->condexec_bits &= 3;

        env->condexec_bits |= (val >> 8) & 0xfc;

    }

    if (mask & CPSR_GE) {

        env->GE = (val >> 16) & 0xf;

    }



    

    if (write_type != CPSRWriteRaw && !arm_feature(env, ARM_FEATURE_V8) &&

        arm_feature(env, ARM_FEATURE_EL3) &&

        !arm_feature(env, ARM_FEATURE_EL2) &&

        !arm_is_secure(env)) {



        changed_daif = (env->daif ^ val) & mask;



        if (changed_daif & CPSR_A) {

            

            if (!(env->cp15.scr_el3 & SCR_AW)) {

                qemu_log_mask(LOG_GUEST_ERROR,

                              "Ignoring attempt to switch CPSR_A flag from "

                              "non-secure world with SCR.AW bit clear\n");

                mask &= ~CPSR_A;

            }

        }



        if (changed_daif & CPSR_F) {

            

            if (!(env->cp15.scr_el3 & SCR_FW)) {

                qemu_log_mask(LOG_GUEST_ERROR,

                              "Ignoring attempt to switch CPSR_F flag from "

                              "non-secure world with SCR.FW bit clear\n");

                mask &= ~CPSR_F;

            }



            

            if ((A32_BANKED_CURRENT_REG_GET(env, sctlr) & SCTLR_NMFI) &&

                (val & CPSR_F)) {

                qemu_log_mask(LOG_GUEST_ERROR,

                              "Ignoring attempt to enable CPSR_F flag "

                              "(non-maskable FIQ [NMFI] support enabled)\n");

                mask &= ~CPSR_F;

            }

        }

    }



    env->daif &= ~(CPSR_AIF & mask);

    env->daif |= val & CPSR_AIF & mask;



    if (write_type != CPSRWriteRaw &&

        (env->uncached_cpsr & CPSR_M) != CPSR_USER &&

        ((env->uncached_cpsr ^ val) & mask & CPSR_M)) {

        if (bad_mode_switch(env, val & CPSR_M)) {

            

            mask &= ~CPSR_M;

        } else {

            switch_mode(env, val & CPSR_M);

        }

    }

    mask &= ~CACHED_CPSR_BITS;

    env->uncached_cpsr = (env->uncached_cpsr & ~mask) | (val & mask);

}