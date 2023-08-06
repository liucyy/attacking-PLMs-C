static void gen_jmpcc(DisasContext *s, int cond, int l1)

{

    TCGv tmp;



    

    gen_flush_flags(s);

    switch (cond) {

    case 0: 

        tcg_gen_br(l1);

        break;

    case 1: 

        break;

    case 2: 

        tmp = tcg_temp_new();

        tcg_gen_andi_i32(tmp, QREG_CC_DEST, CCF_C | CCF_Z);

        tcg_gen_brcondi_i32(TCG_COND_EQ, tmp, 0, l1);

        break;

    case 3: 

        tmp = tcg_temp_new();

        tcg_gen_andi_i32(tmp, QREG_CC_DEST, CCF_C | CCF_Z);

        tcg_gen_brcondi_i32(TCG_COND_NE, tmp, 0, l1);

        break;

    case 4: 

        tmp = tcg_temp_new();

        tcg_gen_andi_i32(tmp, QREG_CC_DEST, CCF_C);

        tcg_gen_brcondi_i32(TCG_COND_EQ, tmp, 0, l1);

        break;

    case 5: 

        tmp = tcg_temp_new();

        tcg_gen_andi_i32(tmp, QREG_CC_DEST, CCF_C);

        tcg_gen_brcondi_i32(TCG_COND_NE, tmp, 0, l1);

        break;

    case 6: 

        tmp = tcg_temp_new();

        tcg_gen_andi_i32(tmp, QREG_CC_DEST, CCF_Z);

        tcg_gen_brcondi_i32(TCG_COND_EQ, tmp, 0, l1);

        break;

    case 7: 

        tmp = tcg_temp_new();

        tcg_gen_andi_i32(tmp, QREG_CC_DEST, CCF_Z);

        tcg_gen_brcondi_i32(TCG_COND_NE, tmp, 0, l1);

        break;

    case 8: 

        tmp = tcg_temp_new();

        tcg_gen_andi_i32(tmp, QREG_CC_DEST, CCF_V);

        tcg_gen_brcondi_i32(TCG_COND_EQ, tmp, 0, l1);

        break;

    case 9: 

        tmp = tcg_temp_new();

        tcg_gen_andi_i32(tmp, QREG_CC_DEST, CCF_V);

        tcg_gen_brcondi_i32(TCG_COND_NE, tmp, 0, l1);

        break;

    case 10: 

        tmp = tcg_temp_new();

        tcg_gen_andi_i32(tmp, QREG_CC_DEST, CCF_N);

        tcg_gen_brcondi_i32(TCG_COND_EQ, tmp, 0, l1);

        break;

    case 11: 

        tmp = tcg_temp_new();

        tcg_gen_andi_i32(tmp, QREG_CC_DEST, CCF_N);

        tcg_gen_brcondi_i32(TCG_COND_NE, tmp, 0, l1);

        break;

    case 12: 

        tmp = tcg_temp_new();

        assert(CCF_V == (CCF_N >> 2));

        tcg_gen_shri_i32(tmp, QREG_CC_DEST, 2);

        tcg_gen_xor_i32(tmp, tmp, QREG_CC_DEST);

        tcg_gen_andi_i32(tmp, tmp, CCF_V);

        tcg_gen_brcondi_i32(TCG_COND_EQ, tmp, 0, l1);

        break;

    case 13: 

        tmp = tcg_temp_new();

        assert(CCF_V == (CCF_N >> 2));

        tcg_gen_shri_i32(tmp, QREG_CC_DEST, 2);

        tcg_gen_xor_i32(tmp, tmp, QREG_CC_DEST);

        tcg_gen_andi_i32(tmp, tmp, CCF_V);

        tcg_gen_brcondi_i32(TCG_COND_NE, tmp, 0, l1);

        break;

    case 14: 

        tmp = tcg_temp_new();

        assert(CCF_V == (CCF_N >> 2));

        tcg_gen_andi_i32(tmp, QREG_CC_DEST, CCF_N);

        tcg_gen_shri_i32(tmp, tmp, 2);

        tcg_gen_xor_i32(tmp, tmp, QREG_CC_DEST);

        tcg_gen_andi_i32(tmp, tmp, CCF_V | CCF_Z);

        tcg_gen_brcondi_i32(TCG_COND_EQ, tmp, 0, l1);

        break;

    case 15: 

        tmp = tcg_temp_new();

        assert(CCF_V == (CCF_N >> 2));

        tcg_gen_andi_i32(tmp, QREG_CC_DEST, CCF_N);

        tcg_gen_shri_i32(tmp, tmp, 2);

        tcg_gen_xor_i32(tmp, tmp, QREG_CC_DEST);

        tcg_gen_andi_i32(tmp, tmp, CCF_V | CCF_Z);

        tcg_gen_brcondi_i32(TCG_COND_NE, tmp, 0, l1);

        break;

    default:

        

        abort();

    }

}