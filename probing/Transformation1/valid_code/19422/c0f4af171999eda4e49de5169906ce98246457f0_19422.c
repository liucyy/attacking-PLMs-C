static void xscale_cpar_write(CPUARMState *env, const ARMCPRegInfo *ri,

                              uint64_t value)

{

    value &= 0x3fff;

    if (env->cp15.c15_cpar != value) {

        

        tb_flush(env);

        env->cp15.c15_cpar = value;

    }

}