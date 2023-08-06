DISAS_INSN(fbcc)

{

    uint32_t offset;

    uint32_t addr;

    TCGv flag;

    int l1;



    addr = s->pc;

    offset = cpu_ldsw_code(env, s->pc);

    s->pc += 2;

    if (insn & (1 << 6)) {

        offset = (offset << 16) | cpu_lduw_code(env, s->pc);

        s->pc += 2;

    }



    l1 = gen_new_label();

    

    flag = tcg_temp_new();

    gen_helper_compare_f64(flag, cpu_env, QREG_FP_RESULT);

    

    switch (insn & 0xf) {

    case 0: 

        break;

    case 1: 

        tcg_gen_brcond_i32(TCG_COND_EQ, flag, tcg_const_i32(0), l1);

        break;

    case 2: 

        tcg_gen_brcond_i32(TCG_COND_EQ, flag, tcg_const_i32(1), l1);

        break;

    case 3: 

        tcg_gen_brcond_i32(TCG_COND_LEU, flag, tcg_const_i32(1), l1);

        break;

    case 4: 

        tcg_gen_brcond_i32(TCG_COND_LT, flag, tcg_const_i32(0), l1);

        break;

    case 5: 

        tcg_gen_brcond_i32(TCG_COND_LE, flag, tcg_const_i32(0), l1);

        break;

    case 6: 

        tcg_gen_andi_i32(flag, flag, 1);

        tcg_gen_brcond_i32(TCG_COND_NE, flag, tcg_const_i32(0), l1);

        break;

    case 7: 

        tcg_gen_brcond_i32(TCG_COND_EQ, flag, tcg_const_i32(2), l1);

        break;

    case 8: 

        tcg_gen_brcond_i32(TCG_COND_LT, flag, tcg_const_i32(2), l1);

        break;

    case 9: 

        tcg_gen_andi_i32(flag, flag, 1);

        tcg_gen_brcond_i32(TCG_COND_EQ, flag, tcg_const_i32(0), l1);

        break;

    case 10: 

        tcg_gen_brcond_i32(TCG_COND_GT, flag, tcg_const_i32(0), l1);

        break;

    case 11: 

        tcg_gen_brcond_i32(TCG_COND_GE, flag, tcg_const_i32(0), l1);

        break;

    case 12: 

        tcg_gen_brcond_i32(TCG_COND_GEU, flag, tcg_const_i32(2), l1);

        break;

    case 13: 

        tcg_gen_brcond_i32(TCG_COND_NE, flag, tcg_const_i32(1), l1);

        break;

    case 14: 

        tcg_gen_brcond_i32(TCG_COND_NE, flag, tcg_const_i32(0), l1);

        break;

    case 15: 

        tcg_gen_br(l1);

        break;

    }

    gen_jmp_tb(s, 0, s->pc);

    gen_set_label(l1);

    gen_jmp_tb(s, 1, addr + offset);

}