static void gen_mtmsr(DisasContext *ctx)

{

#if defined(CONFIG_USER_ONLY)

    gen_inval_exception(ctx, POWERPC_EXCP_PRIV_REG);

#else

    if (unlikely(ctx->pr)) {

        gen_inval_exception(ctx, POWERPC_EXCP_PRIV_REG);

        return;

    }

    if (ctx->opcode & 0x00010000) {

        

        TCGv t0 = tcg_temp_new();

        tcg_gen_andi_tl(t0, cpu_gpr[rS(ctx->opcode)], (1 << MSR_RI) | (1 << MSR_EE));

        tcg_gen_andi_tl(cpu_msr, cpu_msr, ~(target_ulong)((1 << MSR_RI) | (1 << MSR_EE)));

        tcg_gen_or_tl(cpu_msr, cpu_msr, t0);

        tcg_temp_free(t0);

    } else {

        TCGv msr = tcg_temp_new();



        

        gen_update_nip(ctx, ctx->nip);

#if defined(TARGET_PPC64)

        tcg_gen_deposit_tl(msr, cpu_msr, cpu_gpr[rS(ctx->opcode)], 0, 32);

#else

        tcg_gen_mov_tl(msr, cpu_gpr[rS(ctx->opcode)]);

#endif

        gen_helper_store_msr(cpu_env, msr);

        tcg_temp_free(msr);

        

        

        gen_stop_exception(ctx);

    }

#endif

}