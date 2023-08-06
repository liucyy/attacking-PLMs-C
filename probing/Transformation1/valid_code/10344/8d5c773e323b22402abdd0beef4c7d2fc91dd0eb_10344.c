static void contextidr_write(CPUARMState *env, const ARMCPRegInfo *ri,

                             uint64_t value)

{

    ARMCPU *cpu = arm_env_get_cpu(env);



    if (env->cp15.contextidr_el1 != value && !arm_feature(env, ARM_FEATURE_MPU)

        && !extended_addresses_enabled(env)) {

        

        tlb_flush(CPU(cpu), 1);

    }

    env->cp15.contextidr_el1 = value;

}