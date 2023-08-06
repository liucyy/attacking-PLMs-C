static int ppc_hash32_get_physical_address(CPUPPCState *env, struct mmu_ctx_hash32 *ctx,

                                           target_ulong eaddr, int rw,

                                           int access_type)

{

    bool real_mode = (access_type == ACCESS_CODE && msr_ir == 0)

        || (access_type != ACCESS_CODE && msr_dr == 0);



    if (real_mode) {

        ctx->raddr = eaddr;

        ctx->prot = PAGE_READ | PAGE_EXEC | PAGE_WRITE;

        return 0;

    } else {

        int ret = -1;



        

        if (env->nb_BATs != 0) {

            ret = ppc_hash32_get_bat(env, ctx, eaddr, rw, access_type);

        }

        if (ret < 0) {

            

            ret = get_segment32(env, ctx, eaddr, rw, access_type);

        }

        return ret;

    }

}