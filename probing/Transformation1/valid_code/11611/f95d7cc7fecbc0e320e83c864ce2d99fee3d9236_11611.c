static int pte64_check(struct mmu_ctx_hash64 *ctx, target_ulong pte0,

                       target_ulong pte1, int h, int rwx)

{

    target_ulong mmask;

    int access, ret, pp;



    ret = -1;

    

    if ((pte0 & HPTE64_V_VALID) && (h == !!(pte0 & HPTE64_V_SECONDARY))) {

        bool nx;



        

        mmask = PTE64_CHECK_MASK;

        pp = (pte1 & HPTE64_R_PP) | ((pte1 & HPTE64_R_PP0) >> 61);

        

        nx = (pte1 & HPTE64_R_N) || (pte1 & HPTE64_R_G);

        if (HPTE64_V_COMPARE(pte0, ctx->ptem)) {

            if (ctx->raddr != (hwaddr)-1ULL) {

                

                if ((ctx->raddr & mmask) != (pte1 & mmask)) {

                    qemu_log("Bad RPN/WIMG/PP\n");

                    return -3;

                }

            }

            

            access = ppc_hash64_pp_check(ctx->key, pp, nx);

            

            ctx->raddr = pte1;

            ctx->prot = access;

            ret = ppc_hash64_check_prot(ctx->prot, rwx);

            if (ret == 0) {

                

                LOG_MMU("PTE access granted !\n");

            } else {

                

                LOG_MMU("PTE access rejected\n");

            }

        }

    }



    return ret;

}