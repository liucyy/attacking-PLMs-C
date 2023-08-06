static void gen_test_cc(int cc, int label)

{

    TCGv tmp;

    TCGv tmp2;

    int inv;



    switch (cc) {

    case 0: 

        tmp = load_cpu_field(ZF);

        tcg_gen_brcondi_i32(TCG_COND_EQ, tmp, 0, label);

        break;

    case 1: 

        tmp = load_cpu_field(ZF);

        tcg_gen_brcondi_i32(TCG_COND_NE, tmp, 0, label);

        break;

    case 2: 

        tmp = load_cpu_field(CF);

        tcg_gen_brcondi_i32(TCG_COND_NE, tmp, 0, label);

        break;

    case 3: 

        tmp = load_cpu_field(CF);

        tcg_gen_brcondi_i32(TCG_COND_EQ, tmp, 0, label);

        break;

    case 4: 

        tmp = load_cpu_field(NF);

        tcg_gen_brcondi_i32(TCG_COND_LT, tmp, 0, label);

        break;

    case 5: 

        tmp = load_cpu_field(NF);

        tcg_gen_brcondi_i32(TCG_COND_GE, tmp, 0, label);

        break;

    case 6: 

        tmp = load_cpu_field(VF);

        tcg_gen_brcondi_i32(TCG_COND_LT, tmp, 0, label);

        break;

    case 7: 

        tmp = load_cpu_field(VF);

        tcg_gen_brcondi_i32(TCG_COND_GE, tmp, 0, label);

        break;

    case 8: 

        inv = gen_new_label();

        tmp = load_cpu_field(CF);

        tcg_gen_brcondi_i32(TCG_COND_EQ, tmp, 0, inv);

        dead_tmp(tmp);

        tmp = load_cpu_field(ZF);

        tcg_gen_brcondi_i32(TCG_COND_NE, tmp, 0, label);

        gen_set_label(inv);

        break;

    case 9: 

        tmp = load_cpu_field(CF);

        tcg_gen_brcondi_i32(TCG_COND_EQ, tmp, 0, label);

        dead_tmp(tmp);

        tmp = load_cpu_field(ZF);

        tcg_gen_brcondi_i32(TCG_COND_EQ, tmp, 0, label);

        break;

    case 10: 

        tmp = load_cpu_field(VF);

        tmp2 = load_cpu_field(NF);

        tcg_gen_xor_i32(tmp, tmp, tmp2);

        dead_tmp(tmp2);

        tcg_gen_brcondi_i32(TCG_COND_GE, tmp, 0, label);

        break;

    case 11: 

        tmp = load_cpu_field(VF);

        tmp2 = load_cpu_field(NF);

        tcg_gen_xor_i32(tmp, tmp, tmp2);

        dead_tmp(tmp2);

        tcg_gen_brcondi_i32(TCG_COND_LT, tmp, 0, label);

        break;

    case 12: 

        inv = gen_new_label();

        tmp = load_cpu_field(ZF);

        tcg_gen_brcondi_i32(TCG_COND_EQ, tmp, 0, inv);

        dead_tmp(tmp);

        tmp = load_cpu_field(VF);

        tmp2 = load_cpu_field(NF);

        tcg_gen_xor_i32(tmp, tmp, tmp2);

        dead_tmp(tmp2);

        tcg_gen_brcondi_i32(TCG_COND_GE, tmp, 0, label);

        gen_set_label(inv);

        break;

    case 13: 

        tmp = load_cpu_field(ZF);

        tcg_gen_brcondi_i32(TCG_COND_EQ, tmp, 0, label);

        dead_tmp(tmp);

        tmp = load_cpu_field(VF);

        tmp2 = load_cpu_field(NF);

        tcg_gen_xor_i32(tmp, tmp, tmp2);

        dead_tmp(tmp2);

        tcg_gen_brcondi_i32(TCG_COND_LT, tmp, 0, label);

        break;

    default:

        fprintf(stderr, "Bad condition code 0x%x\n", cc);

        abort();

    }

    dead_tmp(tmp);

}