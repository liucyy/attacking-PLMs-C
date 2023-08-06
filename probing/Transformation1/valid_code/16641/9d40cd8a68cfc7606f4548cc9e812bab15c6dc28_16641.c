static void v7m_push_stack(ARMCPU *cpu)

{

    

    CPUARMState *env = &cpu->env;

    uint32_t xpsr = xpsr_read(env);



    

    if ((env->regs[13] & 4) && (env->v7m.ccr & R_V7M_CCR_STKALIGN_MASK)) {

        env->regs[13] -= 4;

        xpsr |= XPSR_SPREALIGN;

    }

    

    v7m_push(env, xpsr);

    v7m_push(env, env->regs[15]);

    v7m_push(env, env->regs[14]);

    v7m_push(env, env->regs[12]);

    v7m_push(env, env->regs[3]);

    v7m_push(env, env->regs[2]);

    v7m_push(env, env->regs[1]);

    v7m_push(env, env->regs[0]);

}