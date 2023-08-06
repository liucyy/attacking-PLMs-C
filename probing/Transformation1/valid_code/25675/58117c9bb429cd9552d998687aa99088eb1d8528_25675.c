void HELPER(v7m_msr)(CPUARMState *env, uint32_t reg, uint32_t val)

{

    ARMCPU *cpu = arm_env_get_cpu(env);



    switch (reg) {

    case 0: 

        xpsr_write(env, val, 0xf8000000);

        break;

    case 1: 

        xpsr_write(env, val, 0xf8000000);

        break;

    case 2: 

        xpsr_write(env, val, 0xfe00fc00);

        break;

    case 3: 

        xpsr_write(env, val, 0xfe00fc00);

        break;

    case 5: 

        

        break;

    case 6: 

        xpsr_write(env, val, 0x0600fc00);

        break;

    case 7: 

        xpsr_write(env, val, 0x0600fc00);

        break;

    case 8: 

        if (env->v7m.current_sp)

            env->v7m.other_sp = val;

        else

            env->regs[13] = val;

        break;

    case 9: 

        if (env->v7m.current_sp)

            env->regs[13] = val;

        else

            env->v7m.other_sp = val;

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

        env->v7m.control = val & 3;

        switch_v7m_sp(env, (val & 2) != 0);

        break;

    default:

        

        cpu_abort(CPU(cpu), "Unimplemented system register write (%d)\n", reg);

        return;

    }

}