static void tcg_out_modrm_sib_offset(TCGContext *s, int opc, int r, int rm,

                                     int index, int shift, intptr_t offset)

{

    int mod, len;



    if (index < 0 && rm < 0) {

        if (TCG_TARGET_REG_BITS == 64) {

            

            intptr_t pc = (intptr_t)s->code_ptr + 5 + ~rm;

            intptr_t disp = offset - pc;

            if (disp == (int32_t)disp) {

                tcg_out_opc(s, opc, r, 0, 0);

                tcg_out8(s, (LOWREGMASK(r) << 3) | 5);

                tcg_out32(s, disp);

                return;

            }



            

            if (offset == (int32_t)offset) {

                tcg_out_opc(s, opc, r, 0, 0);

                tcg_out8(s, (LOWREGMASK(r) << 3) | 4);

                tcg_out8(s, (4 << 3) | 5);

                tcg_out32(s, offset);

                return;

            }



            

            tcg_abort();

        } else {

            

            tcg_out_opc(s, opc, r, 0, 0);

            tcg_out8(s, (r << 3) | 5);

            tcg_out32(s, offset);

            return;

        }

    }



    

    if (rm < 0) {

        mod = 0, len = 4, rm = 5;

    } else if (offset == 0 && LOWREGMASK(rm) != TCG_REG_EBP) {

        mod = 0, len = 0;

    } else if (offset == (int8_t)offset) {

        mod = 0x40, len = 1;

    } else {

        mod = 0x80, len = 4;

    }



    

    if (index < 0 && LOWREGMASK(rm) != TCG_REG_ESP) {

        

        tcg_out_opc(s, opc, r, rm, 0);

        tcg_out8(s, mod | (LOWREGMASK(r) << 3) | LOWREGMASK(rm));

    } else {

        



        

        if (index < 0) {

            index = 4;

        } else {

            assert(index != TCG_REG_ESP);

        }



        tcg_out_opc(s, opc, r, rm, index);

        tcg_out8(s, mod | (LOWREGMASK(r) << 3) | 4);

        tcg_out8(s, (shift << 6) | (LOWREGMASK(index) << 3) | LOWREGMASK(rm));

    }



    if (len == 1) {

        tcg_out8(s, offset);

    } else if (len == 4) {

        tcg_out32(s, offset);

    }

}