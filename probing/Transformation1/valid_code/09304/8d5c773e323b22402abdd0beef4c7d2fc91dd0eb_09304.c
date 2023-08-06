static void vmsa_tcr_el1_write(CPUARMState *env, const ARMCPRegInfo *ri,

                               uint64_t value)

{

    ARMCPU *cpu = arm_env_get_cpu(env);



    

    tlb_flush(CPU(cpu), 1);

    env->cp15.c2_control = value;

}