void HELPER(yield)(CPUARMState *env)

{

    ARMCPU *cpu = arm_env_get_cpu(env);

    CPUState *cs = CPU(cpu);



    

    g_assert(!parallel_cpus);



    

    cs->exception_index = EXCP_YIELD;

    cpu_loop_exit(cs);

}