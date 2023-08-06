static const char *target_parse_constraint(TCGArgConstraint *ct,

                                           const char *ct_str, TCGType type)

{

    switch (*ct_str++) {

    case 'r':                  

        ct->ct |= TCG_CT_REG;

        tcg_regset_set32(ct->u.regs, 0, 0xffff);

        break;

    case 'L':                  

        ct->ct |= TCG_CT_REG;

        tcg_regset_set32(ct->u.regs, 0, 0xffff);

        tcg_regset_reset_reg (ct->u.regs, TCG_REG_R2);

        tcg_regset_reset_reg (ct->u.regs, TCG_REG_R3);

        tcg_regset_reset_reg (ct->u.regs, TCG_REG_R4);

        break;

    case 'a':                  

        ct->ct |= TCG_CT_REG;

        tcg_regset_clear(ct->u.regs);

        tcg_regset_set_reg(ct->u.regs, TCG_REG_R2);

        break;

    case 'b':                  

        ct->ct |= TCG_CT_REG;

        tcg_regset_clear(ct->u.regs);

        tcg_regset_set_reg(ct->u.regs, TCG_REG_R3);

        break;

    case 'A':

        ct->ct |= TCG_CT_CONST_S33;

        break;

    case 'I':

        ct->ct |= TCG_CT_CONST_S16;

        break;

    case 'J':

        ct->ct |= TCG_CT_CONST_S32;

        break;

    case 'O':

        ct->ct |= TCG_CT_CONST_ORI;

        break;

    case 'X':

        ct->ct |= TCG_CT_CONST_XORI;

        break;

    case 'C':

        

        

        ct->ct |= TCG_CT_CONST_U31;

        break;

    case 'Z':

        ct->ct |= TCG_CT_CONST_ZERO;

        break;

    default:

        return NULL;

    }

    return ct_str;

}