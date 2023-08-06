static void fcse_write(CPUARMState *env, const ARMCPRegInfo *ri, uint64_t value)

{

    ARMCPU *cpu = arm_env_get_cpu(env);



    if (env->cp15.c13_fcse != value) {

        

        tlb_flush(CPU(cpu), 1);

        env->cp15.c13_fcse = value;

    }

}