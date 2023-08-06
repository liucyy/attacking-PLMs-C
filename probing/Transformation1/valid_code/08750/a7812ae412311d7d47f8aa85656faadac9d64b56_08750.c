static void t_gen_btst(TCGv d, TCGv a, TCGv b)

{

        TCGv sbit;

        TCGv bset;

        TCGv t0;

	int l1;



        



	l1 = gen_new_label();

        sbit = tcg_temp_new(TCG_TYPE_TL);

        bset = tcg_temp_new(TCG_TYPE_TL);

        t0 = tcg_temp_new(TCG_TYPE_TL);



        

        tcg_gen_andi_tl(sbit, b, 31);

        tcg_gen_shl_tl(t0, tcg_const_tl(1), sbit);

        tcg_gen_and_tl(bset, a, t0);

        tcg_gen_shr_tl(bset, bset, sbit);

	

        tcg_gen_shli_tl(bset, bset, 3);



        tcg_gen_shl_tl(sbit, tcg_const_tl(2), sbit);

        tcg_gen_subi_tl(sbit, sbit, 1);

        tcg_gen_and_tl(sbit, a, sbit);



        tcg_gen_andi_tl(d, cpu_PR[PR_CCS], ~(X_FLAG | N_FLAG | Z_FLAG));

	

        tcg_gen_or_tl(d, d, bset);

	tcg_gen_brcondi_tl(TCG_COND_NE, sbit, 0, l1);

	

	tcg_gen_ori_tl(d, d, Z_FLAG);

	gen_set_label(l1);



        tcg_temp_free(sbit);

        tcg_temp_free(bset);

}