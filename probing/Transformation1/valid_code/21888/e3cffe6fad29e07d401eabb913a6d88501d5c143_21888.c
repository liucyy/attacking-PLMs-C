static void gen_tlbsync(DisasContext *ctx)

{

#if defined(CONFIG_USER_ONLY)

    GEN_PRIV;

#else

    CHK_HV;



    

    gen_check_tlb_flush(ctx);

#endif 

}