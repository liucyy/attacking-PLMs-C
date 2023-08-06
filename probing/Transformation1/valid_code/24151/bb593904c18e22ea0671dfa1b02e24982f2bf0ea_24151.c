void ppc_store_sdr1 (CPUPPCState *env, target_ulong value)

{

    LOG_MMU("%s: " TARGET_FMT_lx "\n", __func__, value);

    if (env->sdr1 != value) {

        

        env->sdr1 = value;

        tlb_flush(env, 1);

    }

}