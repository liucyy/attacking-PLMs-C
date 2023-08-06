static void tcg_out_movi(TCGContext *s, TCGType type,

                         TCGReg ret, tcg_target_long arg)

{

    tcg_target_long hi, lo;



    

    if (check_fit_tl(arg, 13)) {

        tcg_out_movi_imm13(s, ret, arg);

        return;

    }



    

    if (type == TCG_TYPE_I32 || arg == (uint32_t)arg) {

        tcg_out_sethi(s, ret, arg);

        if (arg & 0x3ff) {

            tcg_out_arithi(s, ret, ret, arg & 0x3ff, ARITH_OR);

        }

        return;

    }



    

    if (check_fit_tl(arg, 32)) {

        tcg_out_sethi(s, ret, ~arg);

        tcg_out_arithi(s, ret, ret, (arg & 0x3ff) | -0x400, ARITH_XOR);

        return;

    }



    

    lo = (int32_t)arg;

    if (check_fit_tl(lo, 13)) {

        hi = (arg - lo) >> 32;

        tcg_out_movi(s, TCG_TYPE_I32, ret, hi);

        tcg_out_arithi(s, ret, ret, 32, SHIFT_SLLX);

        tcg_out_arithi(s, ret, ret, lo, ARITH_ADD);

    } else {

        hi = arg >> 32;

        tcg_out_movi(s, TCG_TYPE_I32, ret, hi);

        tcg_out_movi(s, TCG_TYPE_I32, TCG_REG_T2, lo);

        tcg_out_arithi(s, ret, ret, 32, SHIFT_SLLX);

        tcg_out_arith(s, ret, ret, TCG_REG_T2, ARITH_OR);

    }

}