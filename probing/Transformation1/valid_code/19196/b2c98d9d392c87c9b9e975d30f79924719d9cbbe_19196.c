static void tgen_brcond(TCGContext *s, TCGType type, TCGCond c,

                        TCGReg r1, TCGArg c2, int c2const, TCGLabel *l)

{

    int cc;



    if (facilities & FACILITY_GEN_INST_EXT) {

        bool is_unsigned = is_unsigned_cond(c);

        bool in_range;

        S390Opcode opc;



        cc = tcg_cond_to_s390_cond[c];



        if (!c2const) {

            opc = (type == TCG_TYPE_I32

                   ? (is_unsigned ? RIE_CLRJ : RIE_CRJ)

                   : (is_unsigned ? RIE_CLGRJ : RIE_CGRJ));

            tgen_compare_branch(s, opc, cc, r1, c2, l);

            return;

        }



        

        if (type == TCG_TYPE_I32) {

            if (is_unsigned) {

                opc = RIE_CLIJ;

                in_range = (uint32_t)c2 == (uint8_t)c2;

            } else {

                opc = RIE_CIJ;

                in_range = (int32_t)c2 == (int8_t)c2;

            }

        } else {

            if (is_unsigned) {

                opc = RIE_CLGIJ;

                in_range = (uint64_t)c2 == (uint8_t)c2;

            } else {

                opc = RIE_CGIJ;

                in_range = (int64_t)c2 == (int8_t)c2;

            }

        }

        if (in_range) {

            tgen_compare_imm_branch(s, opc, cc, r1, c2, l);

            return;

        }

    }



    cc = tgen_cmp(s, type, c, r1, c2, c2const, false);

    tgen_branch(s, cc, l);

}