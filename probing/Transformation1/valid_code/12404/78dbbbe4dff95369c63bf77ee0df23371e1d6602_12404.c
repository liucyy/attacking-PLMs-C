static int mpidr_read(CPUARMState *env, const ARMCPRegInfo *ri,

                      uint64_t *value)

{

    CPUState *cs = CPU(arm_env_get_cpu(env));

    uint32_t mpidr = cs->cpu_index;

    

    if (arm_feature(env, ARM_FEATURE_V7MP)) {

        mpidr |= (1 << 31);

        

    }

    *value = mpidr;

    return 0;

}