static void gen_sync(DisasContext *ctx)

{

    uint32_t l = (ctx->opcode >> 21) & 3;



    

    if (((l == 2) || !(ctx->insns_flags & PPC_64B)) && !ctx->pr) {

        gen_check_tlb_flush(ctx);

    }

}