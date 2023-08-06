static void gen_jump(DisasContext *dc, uint32_t imm, uint32_t reg, uint32_t op0)

{

    target_ulong tmp_pc;

    

    tmp_pc = sign_extend((imm<<2), 26) + dc->pc;



    switch (op0) {

    case 0x00:     

        tcg_gen_movi_tl(jmp_pc, tmp_pc);

        break;

    case 0x01:     

        tcg_gen_movi_tl(cpu_R[9], (dc->pc + 8));

        tcg_gen_movi_tl(jmp_pc, tmp_pc);

        break;

    case 0x03:     

    case 0x04:     

        {

            int lab = gen_new_label();

            TCGv sr_f = tcg_temp_new();

            tcg_gen_movi_tl(jmp_pc, dc->pc+8);

            tcg_gen_andi_tl(sr_f, cpu_sr, SR_F);

            tcg_gen_brcondi_i32(op0 == 0x03 ? TCG_COND_EQ : TCG_COND_NE,

                                sr_f, SR_F, lab);

            tcg_gen_movi_tl(jmp_pc, tmp_pc);

            gen_set_label(lab);

            tcg_temp_free(sr_f);

        }

        break;

    case 0x11:     

        tcg_gen_mov_tl(jmp_pc, cpu_R[reg]);

        break;

    case 0x12:     

        tcg_gen_movi_tl(cpu_R[9], (dc->pc + 8));

        tcg_gen_mov_tl(jmp_pc, cpu_R[reg]);

        break;

    default:

        gen_illegal_exception(dc);

        break;

    }



    dc->delayed_branch = 2;

    dc->tb_flags |= D_FLAG;

    gen_sync_flags(dc);

}