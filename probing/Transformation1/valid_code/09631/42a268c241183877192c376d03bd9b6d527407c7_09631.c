static ExitStatus gen_store_conditional(DisasContext *ctx, int ra, int rb,

                                        int32_t disp16, int quad)

{

    TCGv addr;



    if (ra == 31) {

        

        return NO_EXIT;

    }



#if defined(CONFIG_USER_ONLY)

    addr = cpu_lock_st_addr;

#else

    addr = tcg_temp_local_new();

#endif



    tcg_gen_addi_i64(addr, load_gpr(ctx, rb), disp16);



#if defined(CONFIG_USER_ONLY)

    

    return gen_excp(ctx, quad ? EXCP_STQ_C : EXCP_STL_C, ra);

#else

    

    {

        int lab_fail, lab_done;

        TCGv val;



        lab_fail = gen_new_label();

        lab_done = gen_new_label();

        tcg_gen_brcond_i64(TCG_COND_NE, addr, cpu_lock_addr, lab_fail);



        val = tcg_temp_new();

        tcg_gen_qemu_ld_i64(val, addr, ctx->mem_idx, quad ? MO_LEQ : MO_LESL);

        tcg_gen_brcond_i64(TCG_COND_NE, val, cpu_lock_value, lab_fail);



        tcg_gen_qemu_st_i64(cpu_ir[ra], addr, ctx->mem_idx,

                            quad ? MO_LEQ : MO_LEUL);

        tcg_gen_movi_i64(cpu_ir[ra], 1);

        tcg_gen_br(lab_done);



        gen_set_label(lab_fail);

        tcg_gen_movi_i64(cpu_ir[ra], 0);



        gen_set_label(lab_done);

        tcg_gen_movi_i64(cpu_lock_addr, -1);



        tcg_temp_free(addr);

        return NO_EXIT;

    }

#endif

}