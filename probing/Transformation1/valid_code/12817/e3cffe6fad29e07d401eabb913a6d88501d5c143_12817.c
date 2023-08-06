static void gen_isync(DisasContext *ctx)

{

    

    if (!ctx->pr) {

        gen_check_tlb_flush(ctx);

    }

    gen_stop_exception(ctx);

}