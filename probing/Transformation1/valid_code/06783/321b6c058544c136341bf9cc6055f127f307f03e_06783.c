static void tcg_out_setcond_i32(TCGContext *s, TCGCond cond, TCGReg ret,

                                TCGReg c1, int32_t c2, int c2const)

{

    

    switch (cond) {

    case TCG_COND_LTU:

    case TCG_COND_GEU:

        

        break;



    case TCG_COND_EQ:

    case TCG_COND_NE:

        

        if (c2 != 0) {

            tcg_out_arithc(s, ret, c1, c2, c2const, ARITH_XOR);

        }

        c1 = TCG_REG_G0, c2 = ret, c2const = 0;

        cond = (cond == TCG_COND_EQ ? TCG_COND_GEU : TCG_COND_LTU);

	break;



    case TCG_COND_GTU:

    case TCG_COND_LEU:

        

        if (!c2const || c2 == 0) {

            TCGReg t = c1;

            c1 = c2;

            c2 = t;

            c2const = 0;

            cond = tcg_swap_cond(cond);

            break;

        }

        



    default:

        tcg_out_cmp(s, c1, c2, c2const);

        tcg_out_movi_imm13(s, ret, 0);

        tcg_out_movcc(s, cond, MOVCC_ICC, ret, 1, 1);

        return;

    }



    tcg_out_cmp(s, c1, c2, c2const);

    if (cond == TCG_COND_LTU) {

        tcg_out_arithi(s, ret, TCG_REG_G0, 0, ARITH_ADDX);

    } else {

        tcg_out_arithi(s, ret, TCG_REG_G0, -1, ARITH_SUBX);

    }

}