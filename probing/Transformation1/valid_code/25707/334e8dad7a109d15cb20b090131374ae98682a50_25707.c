static void do_v7m_exception_exit(ARMCPU *cpu)

{

    CPUARMState *env = &cpu->env;

    uint32_t type;

    uint32_t xpsr;

    bool ufault = false;

    bool return_to_sp_process = false;

    bool return_to_handler = false;

    bool rettobase = false;



    

    assert(arm_v7m_is_handler_mode(env));



    

    type = env->regs[15];

    if (env->thumb) {

        type |= 1;

    }



    qemu_log_mask(CPU_LOG_INT, "Exception return: magic PC %" PRIx32

                  " previous exception %d\n",

                  type, env->v7m.exception);



    if (extract32(type, 5, 23) != extract32(-1, 5, 23)) {

        qemu_log_mask(LOG_GUEST_ERROR, "M profile: zero high bits in exception "

                      "exit PC value 0x%" PRIx32 " are UNPREDICTABLE\n", type);

    }



    if (env->v7m.exception != ARMV7M_EXCP_NMI) {

        

        if (arm_feature(env, ARM_FEATURE_M_SECURITY)) {

            int es = type & 1;

            if (armv7m_nvic_raw_execution_priority(env->nvic) >= 0) {

                env->v7m.faultmask[es] = 0;

            }

        } else {

            env->v7m.faultmask[M_REG_NS] = 0;

        }

    }



    switch (armv7m_nvic_complete_irq(env->nvic, env->v7m.exception)) {

    case -1:

        

        ufault = true;

        break;

    case 0:

        

        break;

    case 1:

        

        rettobase = true;

        break;

    default:

        g_assert_not_reached();

    }



    switch (type & 0xf) {

    case 1: 

        return_to_handler = true;

        break;

    case 13: 

        return_to_sp_process = true;

        

    case 9: 

        if (!rettobase &&

            !(env->v7m.ccr[env->v7m.secure] & R_V7M_CCR_NONBASETHRDENA_MASK)) {

            ufault = true;

        }

        break;

    default:

        ufault = true;

    }



    if (ufault) {

        

        env->v7m.cfsr |= R_V7M_CFSR_INVPC_MASK;

        armv7m_nvic_set_pending(env->nvic, ARMV7M_EXCP_USAGE);

        v7m_exception_taken(cpu, type | 0xf0000000);

        qemu_log_mask(CPU_LOG_INT, "...taking UsageFault on existing "

                      "stackframe: failed exception return integrity check\n");

        return;

    }



    

    switch_v7m_sp(env, return_to_sp_process);

    

    env->regs[0] = v7m_pop(env);

    env->regs[1] = v7m_pop(env);

    env->regs[2] = v7m_pop(env);

    env->regs[3] = v7m_pop(env);

    env->regs[12] = v7m_pop(env);

    env->regs[14] = v7m_pop(env);

    env->regs[15] = v7m_pop(env);

    if (env->regs[15] & 1) {

        qemu_log_mask(LOG_GUEST_ERROR,

                      "M profile return from interrupt with misaligned "

                      "PC is UNPREDICTABLE\n");

        

        env->regs[15] &= ~1U;

    }

    xpsr = v7m_pop(env);

    xpsr_write(env, xpsr, ~XPSR_SPREALIGN);

    

    if (xpsr & XPSR_SPREALIGN) {

        env->regs[13] |= 4;

    }



    

    if (return_to_handler != arm_v7m_is_handler_mode(env)) {

        

        armv7m_nvic_set_pending(env->nvic, ARMV7M_EXCP_USAGE);

        env->v7m.cfsr |= R_V7M_CFSR_INVPC_MASK;

        v7m_push_stack(cpu);

        v7m_exception_taken(cpu, type | 0xf0000000);

        qemu_log_mask(CPU_LOG_INT, "...taking UsageFault on new stackframe: "

                      "failed exception return integrity check\n");

        return;

    }



    

    qemu_log_mask(CPU_LOG_INT, "...successful exception return\n");

}