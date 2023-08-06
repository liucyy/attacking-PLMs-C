static unsigned int dec_rfe_etc(DisasContext *dc)

{

	cris_cc_mask(dc, 0);



	if (dc->op2 == 15) 

		return 2;



	switch (dc->op2 & 7) {

		case 2:

			

			DIS(fprintf(logfile, "rfe\n"));

			cris_evaluate_flags(dc);

			tcg_gen_helper_0_0(helper_rfe);

			dc->is_jmp = DISAS_UPDATE;

			break;

		case 5:

			

			DIS(fprintf(logfile, "rfn\n"));

			cris_evaluate_flags(dc);

			tcg_gen_helper_0_0(helper_rfn);

			dc->is_jmp = DISAS_UPDATE;

			break;

		case 6:

			DIS(fprintf(logfile, "break %d\n", dc->op1));

			cris_evaluate_flags (dc);

			

			tcg_gen_movi_tl(env_pc, dc->pc + 2);



			

			t_gen_mov_env_TN(trap_vector, 

					 tcg_const_tl(dc->op1 + 16));

			t_gen_raise_exception(EXCP_BREAK);

			dc->is_jmp = DISAS_UPDATE;

			break;

		default:

			printf ("op2=%x\n", dc->op2);

			BUG();

			break;



	}

	return 2;

}