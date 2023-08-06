static void gen_mtmsrd(DisasContext *ctx)

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

        

        gen_update_nip(ctx, ctx->nip);

        gen_helper_store_msr(cpu_env, cpu_gpr[rS(ctx->opcode)]);

        

        

        gen_stop_exception(ctx);

    }

#endif

}