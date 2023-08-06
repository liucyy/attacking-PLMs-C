static CPAccessResult pmreg_access(CPUARMState *env, const ARMCPRegInfo *ri,

                                   bool isread)

{

    

    if (arm_current_el(env) == 0 && !env->cp15.c9_pmuserenr) {

        return CP_ACCESS_TRAP;

    }

    return CP_ACCESS_OK;

}