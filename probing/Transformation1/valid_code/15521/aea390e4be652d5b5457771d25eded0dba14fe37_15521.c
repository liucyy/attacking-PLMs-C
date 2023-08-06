static int ppc_hash64_pte_update_flags(struct mmu_ctx_hash64 *ctx,

                                       target_ulong *pte1p,

                                       int ret, int rw)

{

    int store = 0;



    

    if (!(*pte1p & HPTE64_R_R)) {

        

        *pte1p |= HPTE64_R_R;

        store = 1;

    }

    if (!(*pte1p & HPTE64_R_C)) {

        if (rw == 1 && ret == 0) {

            

            *pte1p |= HPTE64_R_C;

            store = 1;

        } else {

            

            ctx->prot &= ~PAGE_WRITE;

        }

    }



    return store;

}