void HELPER(wfe)(CPUARMState *env)

{

    CPUState *cs = CPU(arm_env_get_cpu(env));



    

    cs->exception_index = EXCP_YIELD;

    cpu_loop_exit(cs);

}