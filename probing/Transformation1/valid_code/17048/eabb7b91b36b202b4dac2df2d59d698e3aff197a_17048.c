static void tcg_out_logicali(TCGContext *s, AArch64Insn insn, TCGType ext,

                             TCGReg rd, TCGReg rn, uint64_t limm)

{

    unsigned h, l, r, c;



    assert(is_limm(limm));



    h = clz64(limm);

    l = ctz64(limm);

    if (l == 0) {

        r = 0;                  

        c = ctz64(~limm) - 1;

        if (h == 0) {

            r = clz64(~limm);   

            c += r;

        }

    } else {

        r = 64 - l;             

        c = r - h - 1;

    }

    if (ext == TCG_TYPE_I32) {

        r &= 31;

        c &= 31;

    }



    tcg_out_insn_3404(s, insn, ext, rd, rn, ext, r, c);

}