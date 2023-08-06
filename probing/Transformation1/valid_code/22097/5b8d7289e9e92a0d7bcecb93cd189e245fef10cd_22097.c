static bool insn_crosses_page(CPUARMState *env, DisasContext *s)

{

    

    uint16_t insn;



    if ((s->pc & 3) == 0) {

        

        return false;

    }



    

    insn = arm_lduw_code(env, s->pc, s->sctlr_b);



    if ((insn >> 11) >= 0x1d) {

        

        return true;

    }

    

    return false;

}