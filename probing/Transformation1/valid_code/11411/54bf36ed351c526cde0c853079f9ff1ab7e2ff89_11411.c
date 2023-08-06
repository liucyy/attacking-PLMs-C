do_kernel_trap(CPUARMState *env)

{

    uint32_t addr;

    uint32_t cpsr;

    uint32_t val;



    switch (env->regs[15]) {

    case 0xffff0fa0: 

        

        break;

    case 0xffff0fc0: 

         

        start_exclusive();

        cpsr = cpsr_read(env);

        addr = env->regs[2];

        

        if (get_user_u32(val, addr))

            val = ~env->regs[0];

        if (val == env->regs[0]) {

            val = env->regs[1];

            

            put_user_u32(val, addr);

            env->regs[0] = 0;

            cpsr |= CPSR_C;

        } else {

            env->regs[0] = -1;

            cpsr &= ~CPSR_C;

        }

        cpsr_write(env, cpsr, CPSR_C);

        end_exclusive();

        break;

    case 0xffff0fe0: 

        env->regs[0] = env->cp15.tpidrro_el0;

        break;

    case 0xffff0f60: 

        arm_kernel_cmpxchg64_helper(env);

        break;



    default:

        return 1;

    }

    

    addr = env->regs[14];

    if (addr & 1) {

        env->thumb = 1;

        addr &= ~1;

    }

    env->regs[15] = addr;



    return 0;

}