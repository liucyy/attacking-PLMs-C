static void gen_mtdcrx(DisasContext *ctx)

{

#if defined(CONFIG_USER_ONLY)

    gen_inval_exception(ctx, POWERPC_EXCP_PRIV_REG);

#else

    if (unlikely(ctx->pr)) {

        gen_inval_exception(ctx, POWERPC_EXCP_PRIV_REG);

        return;

    }

    

    gen_update_nip(ctx, ctx->nip - 4);

    gen_helper_store_dcr(cpu_env, cpu_gpr[rA(ctx->opcode)],

                         cpu_gpr[rS(ctx->opcode)]);

    

#endif

}