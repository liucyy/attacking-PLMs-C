void arm_gen_test_cc(int cc, int label)

{

    TCGv_i32 tmp;

    int inv;



    switch (cc) {

    case 0: 

        tcg_gen_brcondi_i32(TCG_COND_EQ, cpu_ZF, 0, label);

        break;

    case 1: 

        tcg_gen_brcondi_i32(TCG_COND_NE, cpu_ZF, 0, label);

        break;

    case 2: 

        tcg_gen_brcondi_i32(TCG_COND_NE, cpu_CF, 0, label);

        break;

    case 3: 

        tcg_gen_brcondi_i32(TCG_COND_EQ, cpu_CF, 0, label);

        break;

    case 4: 

        tcg_gen_brcondi_i32(TCG_COND_LT, cpu_NF, 0, label);

        break;

    case 5: 

        tcg_gen_brcondi_i32(TCG_COND_GE, cpu_NF, 0, label);

        break;

    case 6: 

        tcg_gen_brcondi_i32(TCG_COND_LT, cpu_VF, 0, label);

        break;

    case 7: 

        tcg_gen_brcondi_i32(TCG_COND_GE, cpu_VF, 0, label);

        break;

    case 8: 

        inv = gen_new_label();

        tcg_gen_brcondi_i32(TCG_COND_EQ, cpu_CF, 0, inv);

        tcg_gen_brcondi_i32(TCG_COND_NE, cpu_ZF, 0, label);

        gen_set_label(inv);

        break;

    case 9: 

        tcg_gen_brcondi_i32(TCG_COND_EQ, cpu_CF, 0, label);

        tcg_gen_brcondi_i32(TCG_COND_EQ, cpu_ZF, 0, label);

        break;

    case 10: 

        tmp = tcg_temp_new_i32();

        tcg_gen_xor_i32(tmp, cpu_VF, cpu_NF);

        tcg_gen_brcondi_i32(TCG_COND_GE, tmp, 0, label);

        tcg_temp_free_i32(tmp);

        break;

    case 11: 

        tmp = tcg_temp_new_i32();

        tcg_gen_xor_i32(tmp, cpu_VF, cpu_NF);

        tcg_gen_brcondi_i32(TCG_COND_LT, tmp, 0, label);

        tcg_temp_free_i32(tmp);

        break;

    case 12: 

        inv = gen_new_label();

        tcg_gen_brcondi_i32(TCG_COND_EQ, cpu_ZF, 0, inv);

        tmp = tcg_temp_new_i32();

        tcg_gen_xor_i32(tmp, cpu_VF, cpu_NF);

        tcg_gen_brcondi_i32(TCG_COND_GE, tmp, 0, label);

        tcg_temp_free_i32(tmp);

        gen_set_label(inv);

        break;

    case 13: 

        tcg_gen_brcondi_i32(TCG_COND_EQ, cpu_ZF, 0, label);

        tmp = tcg_temp_new_i32();

        tcg_gen_xor_i32(tmp, cpu_VF, cpu_NF);

        tcg_gen_brcondi_i32(TCG_COND_LT, tmp, 0, label);

        tcg_temp_free_i32(tmp);

        break;

    default:

        fprintf(stderr, "Bad condition code 0x%x\n", cc);

        abort();

    }

}