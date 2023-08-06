static void sctlr_write(CPUARMState *env, const ARMCPRegInfo *ri,

                        uint64_t value)

{

    ARMCPU *cpu = arm_env_get_cpu(env);



    if (env->cp15.c1_sys == value) {

        

        return;

    }



    env->cp15.c1_sys = value;

    

    

    tlb_flush(CPU(cpu), 1);

}