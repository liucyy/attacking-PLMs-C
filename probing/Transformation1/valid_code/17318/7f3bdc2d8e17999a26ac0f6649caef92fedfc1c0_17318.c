static int ppc_hash32_translate(CPUPPCState *env, struct mmu_ctx_hash32 *ctx,

                                target_ulong eaddr, int rwx)

{

    int ret;

    target_ulong sr;



    

    if (((rwx == 2) && (msr_ir == 0)) || ((rwx != 2) && (msr_dr == 0))) {

        

        ctx->raddr = eaddr;

        ctx->prot = PAGE_READ | PAGE_EXEC | PAGE_WRITE;

        return 0;

    }



    

    if (env->nb_BATs != 0) {

        ret = ppc_hash32_get_bat(env, ctx, eaddr, rwx);

        if (ret == 0) {

            return 0;

        }

    }



    

    sr = env->sr[eaddr >> 28];



    

    if (sr & SR32_T) {

        return ppc_hash32_direct_store(env, sr, eaddr, rwx,

                                       &ctx->raddr, &ctx->prot);

    }



    

    ctx->nx = !!(sr & SR32_NX);

    if ((rwx == 2) && ctx->nx) {

        return -3;

    }

    ret = find_pte32(env, ctx, sr, eaddr, rwx);



    return ret;

}