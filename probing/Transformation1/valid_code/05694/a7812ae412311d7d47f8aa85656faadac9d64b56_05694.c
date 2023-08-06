static void gen_tst_cc (DisasContext *dc, TCGv cc, int cond)

{

	int arith_opt, move_opt;



	



	

	arith_opt = arith_cc(dc) && !dc->flags_uptodate;

	move_opt = (dc->cc_op == CC_OP_MOVE) && dc->flags_uptodate;

	switch (cond) {

		case CC_EQ:

			if (arith_opt || move_opt) {

				

				int l1;

				l1 = gen_new_label();

				tcg_gen_movi_tl(cc, 0);

				tcg_gen_brcondi_tl(TCG_COND_NE, cc_result, 

						   0, l1);

				tcg_gen_movi_tl(cc, 1);

				gen_set_label(l1);

			}

			else {

				cris_evaluate_flags(dc);

				tcg_gen_andi_tl(cc, 

						cpu_PR[PR_CCS], Z_FLAG);

			}

			break;

		case CC_NE:

			if (arith_opt || move_opt)

				tcg_gen_mov_tl(cc, cc_result);

			else {

				cris_evaluate_flags(dc);

				tcg_gen_xori_tl(cc, cpu_PR[PR_CCS],

						Z_FLAG);

				tcg_gen_andi_tl(cc, cc, Z_FLAG);

			}

			break;

		case CC_CS:

			cris_evaluate_flags(dc);

			tcg_gen_andi_tl(cc, cpu_PR[PR_CCS], C_FLAG);

			break;

		case CC_CC:

			cris_evaluate_flags(dc);

			tcg_gen_xori_tl(cc, cpu_PR[PR_CCS], C_FLAG);

			tcg_gen_andi_tl(cc, cc, C_FLAG);

			break;

		case CC_VS:

			cris_evaluate_flags(dc);

			tcg_gen_andi_tl(cc, cpu_PR[PR_CCS], V_FLAG);

			break;

		case CC_VC:

			cris_evaluate_flags(dc);

			tcg_gen_xori_tl(cc, cpu_PR[PR_CCS],

					V_FLAG);

			tcg_gen_andi_tl(cc, cc, V_FLAG);

			break;

		case CC_PL:

			if (arith_opt || move_opt) {

				int bits = 31;



				if (dc->cc_size == 1)

					bits = 7;

				else if (dc->cc_size == 2)

					bits = 15;	



				tcg_gen_shri_tl(cc, cc_result, bits);

				tcg_gen_xori_tl(cc, cc, 1);

			} else {

				cris_evaluate_flags(dc);

				tcg_gen_xori_tl(cc, cpu_PR[PR_CCS],

						N_FLAG);

				tcg_gen_andi_tl(cc, cc, N_FLAG);

			}

			break;

		case CC_MI:

			if (arith_opt || move_opt) {

				int bits = 31;



				if (dc->cc_size == 1)

					bits = 7;

				else if (dc->cc_size == 2)

					bits = 15;	



				tcg_gen_shri_tl(cc, cc_result, 31);

			}

			else {

				cris_evaluate_flags(dc);

				tcg_gen_andi_tl(cc, cpu_PR[PR_CCS],

						N_FLAG);

			}

			break;

		case CC_LS:

			cris_evaluate_flags(dc);

			tcg_gen_andi_tl(cc, cpu_PR[PR_CCS],

					C_FLAG | Z_FLAG);

			break;

		case CC_HI:

			cris_evaluate_flags(dc);

			{

				TCGv tmp;



				tmp = tcg_temp_new(TCG_TYPE_TL);

				tcg_gen_xori_tl(tmp, cpu_PR[PR_CCS],

						C_FLAG | Z_FLAG);

				

				tcg_gen_shli_tl(cc, tmp, 2);

				tcg_gen_and_tl(cc, tmp, cc);

				tcg_gen_andi_tl(cc, cc, Z_FLAG);



				tcg_temp_free(tmp);

			}

			break;

		case CC_GE:

			cris_evaluate_flags(dc);

			

			tcg_gen_shli_tl(cc, cpu_PR[PR_CCS], 2);

			tcg_gen_xor_tl(cc,

				       cpu_PR[PR_CCS], cc);

			tcg_gen_andi_tl(cc, cc, N_FLAG);

			tcg_gen_xori_tl(cc, cc, N_FLAG);

			break;

		case CC_LT:

			cris_evaluate_flags(dc);

			

			tcg_gen_shli_tl(cc, cpu_PR[PR_CCS], 2);

			tcg_gen_xor_tl(cc,

				       cpu_PR[PR_CCS], cc);

			tcg_gen_andi_tl(cc, cc, N_FLAG);

			break;

		case CC_GT:

			cris_evaluate_flags(dc);

			{

				TCGv n, z;



				n = tcg_temp_new(TCG_TYPE_TL);

				z = tcg_temp_new(TCG_TYPE_TL);



				

				tcg_gen_shri_tl(n, cpu_PR[PR_CCS], 2);

				tcg_gen_shri_tl(z, cpu_PR[PR_CCS], 1);

				

				tcg_gen_xori_tl(z, z, 2);



				tcg_gen_xor_tl(n, n, cpu_PR[PR_CCS]);

				tcg_gen_xori_tl(n, n, 2);

				tcg_gen_and_tl(cc, z, n);

				tcg_gen_andi_tl(cc, cc, 2);



				tcg_temp_free(n);

				tcg_temp_free(z);

			}

			break;

		case CC_LE:

			cris_evaluate_flags(dc);

			{

				TCGv n, z;



				n = tcg_temp_new(TCG_TYPE_TL);

				z = tcg_temp_new(TCG_TYPE_TL);



				

				tcg_gen_shri_tl(n, cpu_PR[PR_CCS], 2);

				tcg_gen_shri_tl(z, cpu_PR[PR_CCS], 1);



				tcg_gen_xor_tl(n, n, cpu_PR[PR_CCS]);

				tcg_gen_or_tl(cc, z, n);

				tcg_gen_andi_tl(cc, cc, 2);



				tcg_temp_free(n);

				tcg_temp_free(z);

			}

			break;

		case CC_P:

			cris_evaluate_flags(dc);

			tcg_gen_andi_tl(cc, cpu_PR[PR_CCS], P_FLAG);

			break;

		case CC_A:

			tcg_gen_movi_tl(cc, 1);

			break;

		default:

			BUG();

			break;

	};

}