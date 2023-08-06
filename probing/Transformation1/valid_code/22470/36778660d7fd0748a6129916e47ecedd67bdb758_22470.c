static hwaddr ppc_hash64_pteg_search(PowerPCCPU *cpu, hwaddr hash,

                                     const struct ppc_one_seg_page_size *sps,

                                     target_ulong ptem,

                                     ppc_hash_pte64_t *pte, unsigned *pshift)

{

    CPUPPCState *env = &cpu->env;

    int i;

    const ppc_hash_pte64_t *pteg;

    target_ulong pte0, pte1;

    target_ulong ptex;



    ptex = (hash & env->htab_mask) * HPTES_PER_GROUP;

    pteg = ppc_hash64_map_hptes(cpu, ptex, HPTES_PER_GROUP);

    if (!pteg) {

        return -1;

    }

    for (i = 0; i < HPTES_PER_GROUP; i++) {

        pte0 = ppc_hash64_hpte0(cpu, pteg, i);

        pte1 = ppc_hash64_hpte1(cpu, pteg, i);



        

        if (HPTE64_V_COMPARE(pte0, ptem)) {

            *pshift = hpte_page_shift(sps, pte0, pte1);

            

            if (*pshift == 0) {

                continue;

            }

            

            pte->pte0 = pte0;

            pte->pte1 = pte1;

            ppc_hash64_unmap_hptes(cpu, pteg, ptex, HPTES_PER_GROUP);

            return ptex + i;

        }

    }

    ppc_hash64_unmap_hptes(cpu, pteg, ptex, HPTES_PER_GROUP);

    

    return -1;

}