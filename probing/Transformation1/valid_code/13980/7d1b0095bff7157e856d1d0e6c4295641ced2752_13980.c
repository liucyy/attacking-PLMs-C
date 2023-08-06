static int disas_cp15_insn(CPUState *env, DisasContext *s, uint32_t insn)

{

    uint32_t rd;

    TCGv tmp, tmp2;



    

    if (arm_feature(env, ARM_FEATURE_M))

	return 1;



    if ((insn & (1 << 25)) == 0) {

        if (insn & (1 << 20)) {

            

            return 1;

        }

        

        return 0;

    }

    if ((insn & (1 << 4)) == 0) {

        

        return 1;

    }

    if (IS_USER(s) && !cp15_user_ok(insn)) {

        return 1;

    }



    

    if ((insn & 0x0fff0fff) == 0x0e070f90) {

        

        if (!arm_feature(env, ARM_FEATURE_V7)) {

            

            gen_set_pc_im(s->pc);

            s->is_jmp = DISAS_WFI;

        }

        return 0;

    }



    if ((insn & 0x0fff0fff) == 0x0e070f58) {

        

        if (!arm_feature(env, ARM_FEATURE_V6)) {

            

            gen_set_pc_im(s->pc);

            s->is_jmp = DISAS_WFI;

            return 0;

        }

        

    }



    rd = (insn >> 12) & 0xf;



    if (cp15_tls_load_store(env, s, insn, rd))

        return 0;



    tmp2 = tcg_const_i32(insn);

    if (insn & ARM_CP_RW_BIT) {

        tmp = new_tmp();

        gen_helper_get_cp15(tmp, cpu_env, tmp2);

        

        if (rd != 15)

            store_reg(s, rd, tmp);

        else

            dead_tmp(tmp);

    } else {

        tmp = load_reg(s, rd);

        gen_helper_set_cp15(cpu_env, tmp2, tmp);

        dead_tmp(tmp);

        

        if (!arm_feature(env, ARM_FEATURE_XSCALE) ||

                (insn & 0x0fff0fff) != 0x0e010f10)

            gen_lookup_tb(s);

    }

    tcg_temp_free_i32(tmp2);

    return 0;

}