static CPAccessResult ats_access(CPUARMState *env, const ARMCPRegInfo *ri)

{

    if (ri->opc2 & 4) {

        

        return CP_ACCESS_TRAP_UNCATEGORIZED;

    }

    return CP_ACCESS_OK;

}