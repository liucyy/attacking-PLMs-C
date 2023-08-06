static void tcg_out_movi(TCGContext *s, TCGType type,

                         TCGReg ret, tcg_target_long sval)

{

    static const S390Opcode lli_insns[4] = {

        RI_LLILL, RI_LLILH, RI_LLIHL, RI_LLIHH

    };



    tcg_target_ulong uval = sval;

    int i;



    if (type == TCG_TYPE_I32) {

        uval = (uint32_t)sval;

        sval = (int32_t)sval;

    }



    

    if (sval >= -0x8000 && sval < 0x8000) {

        tcg_out_insn(s, RI, LGHI, ret, sval);

        return;

    }



    for (i = 0; i < 4; i++) {

        tcg_target_long mask = 0xffffull << i*16;

        if ((uval & mask) == uval) {

            tcg_out_insn_RI(s, lli_insns[i], ret, uval >> i*16);

            return;

        }

    }



    

    if (facilities & FACILITY_EXT_IMM) {

        if (sval == (int32_t)sval) {

            tcg_out_insn(s, RIL, LGFI, ret, sval);

            return;

        }

        if (uval <= 0xffffffff) {

            tcg_out_insn(s, RIL, LLILF, ret, uval);

            return;

        }

        if ((uval & 0xffffffff) == 0) {

            tcg_out_insn(s, RIL, LLIHF, ret, uval >> 31 >> 1);

            return;

        }

    }



    

    if ((sval & 1) == 0) {

        ptrdiff_t off = tcg_pcrel_diff(s, (void *)sval) >> 1;

        if (off == (int32_t)off) {

            tcg_out_insn(s, RIL, LARL, ret, off);

            return;

        }

    }



    

    if (!(facilities & FACILITY_EXT_IMM)) {

        

        if (uval <= 0xffffffff) {

            tcg_out_insn(s, RI, LLILL, ret, uval);

            tcg_out_insn(s, RI, IILH, ret, uval >> 16);

            return;

        }



        

        if (sval >> 31 >> 1 == -1) {

            if (uval & 0x8000) {

                tcg_out_insn(s, RI, LGHI, ret, uval);

            } else {

                tcg_out_insn(s, RI, LGHI, ret, -1);

                tcg_out_insn(s, RI, IILL, ret, uval);

            }

            tcg_out_insn(s, RI, IILH, ret, uval >> 16);

            return;

        }

    }



    



    

    tcg_out_movi(s, TCG_TYPE_I64, ret, uval & 0xffffffff);



    

    uval = uval >> 31 >> 1;

    if (facilities & FACILITY_EXT_IMM) {

        if (uval < 0x10000) {

            tcg_out_insn(s, RI, IIHL, ret, uval);

        } else if ((uval & 0xffff) == 0) {

            tcg_out_insn(s, RI, IIHH, ret, uval >> 16);

        } else {

            tcg_out_insn(s, RIL, IIHF, ret, uval);

        }

    } else {

        if (uval & 0xffff) {

            tcg_out_insn(s, RI, IIHL, ret, uval);

        }

        if (uval & 0xffff0000) {

            tcg_out_insn(s, RI, IIHH, ret, uval >> 16);

        }

    }

}