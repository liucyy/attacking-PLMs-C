static unsigned int crisv32_decoder(CPUCRISState *env, DisasContext *dc)

{

    int insn_len = 2;

    int i;



    if (unlikely(qemu_loglevel_mask(CPU_LOG_TB_OP | CPU_LOG_TB_OP_OPT))) {

        tcg_gen_debug_insn_start(dc->pc);

        }



    

        dc->ir = cris_fetch(env, dc, dc->pc, 2, 0);



    

    dc->opcode   = EXTRACT_FIELD(dc->ir, 4, 11);

    dc->op1      = EXTRACT_FIELD(dc->ir, 0, 3);

    dc->op2      = EXTRACT_FIELD(dc->ir, 12, 15);

    dc->zsize    = EXTRACT_FIELD(dc->ir, 4, 4);

    dc->zzsize   = EXTRACT_FIELD(dc->ir, 4, 5);

    dc->postinc  = EXTRACT_FIELD(dc->ir, 10, 10);



    

    for (i = 0; i < ARRAY_SIZE(decinfo); i++) {

        if ((dc->opcode & decinfo[i].mask) == decinfo[i].bits) {

            insn_len = decinfo[i].dec(env, dc);

            break;

        }

    }



#if !defined(CONFIG_USER_ONLY)

    

    if (dc->tb_flags & S_FLAG) {

        int l1;



        l1 = gen_new_label();

        tcg_gen_brcondi_tl(TCG_COND_NE, cpu_PR[PR_SPC], dc->pc, l1);

        

        cris_evaluate_flags(dc);

        t_gen_mov_env_TN(trap_vector, tcg_const_tl(3));

        tcg_gen_movi_tl(env_pc, dc->pc + insn_len);

        tcg_gen_movi_tl(cpu_PR[PR_SPC], dc->pc + insn_len);

        t_gen_raise_exception(EXCP_BREAK);

        gen_set_label(l1);

    }

#endif

    return insn_len;

}