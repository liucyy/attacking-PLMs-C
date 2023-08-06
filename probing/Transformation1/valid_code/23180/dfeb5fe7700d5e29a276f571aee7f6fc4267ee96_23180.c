static void tcg_out_movi(TCGContext *s, TCGType type, TCGReg rd,

                         tcg_target_long value)

{

    AArch64Insn insn;



    if (type == TCG_TYPE_I32) {

        value = (uint32_t)value;

    }



    

    insn = I3405_MOVZ;

    do {

        unsigned shift = ctz64(value) & (63 & -16);

        tcg_out_insn_3405(s, insn, shift >= 32, rd, value >> shift, shift);

        value &= ~(0xffffUL << shift);

        insn = I3405_MOVK;

    } while (value);

}