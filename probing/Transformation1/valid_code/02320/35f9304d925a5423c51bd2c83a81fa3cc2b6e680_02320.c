static target_ulong h_enter(CPUPPCState *env, sPAPREnvironment *spapr,

                            target_ulong opcode, target_ulong *args)

{

    target_ulong flags = args[0];

    target_ulong pte_index = args[1];

    target_ulong pteh = args[2];

    target_ulong ptel = args[3];

    target_ulong page_shift = 12;

    target_ulong raddr;

    target_ulong i;

    uint8_t *hpte;



    

    if (pteh & HPTE_V_LARGE) {

#if 0 

        if ((ptel & 0xf000) == 0x1000) {

            

        } else

#endif

        if ((ptel & 0xff000) == 0) {

            

            page_shift = 24;

            

            if (pteh & 0x80) {

                return H_PARAMETER;

            }

        } else {

            return H_PARAMETER;

        }

    }



    raddr = (ptel & HPTE_R_RPN) & ~((1ULL << page_shift) - 1);



    if (raddr < spapr->ram_limit) {

        

        if ((ptel & HPTE_R_WIMG) != HPTE_R_M) {

            return H_PARAMETER;

        }

    } else {

        

        

        

        if ((ptel & (HPTE_R_W | HPTE_R_I | HPTE_R_M)) != HPTE_R_I) {

            return H_PARAMETER;

        }

    }



    pteh &= ~0x60ULL;



    if ((pte_index * HASH_PTE_SIZE_64) & ~env->htab_mask) {

        return H_PARAMETER;

    }

    if (likely((flags & H_EXACT) == 0)) {

        pte_index &= ~7ULL;

        hpte = env->external_htab + (pte_index * HASH_PTE_SIZE_64);

        for (i = 0; ; ++i) {

            if (i == 8) {

                return H_PTEG_FULL;

            }

            if (((ldq_p(hpte) & HPTE_V_VALID) == 0) &&

                lock_hpte(hpte, HPTE_V_HVLOCK | HPTE_V_VALID)) {

                break;

            }

            hpte += HASH_PTE_SIZE_64;

        }

    } else {

        i = 0;

        hpte = env->external_htab + (pte_index * HASH_PTE_SIZE_64);

        if (!lock_hpte(hpte, HPTE_V_HVLOCK | HPTE_V_VALID)) {

            return H_PTEG_FULL;

        }

    }

    stq_p(hpte + (HASH_PTE_SIZE_64/2), ptel);

    

    stq_p(hpte, pteh);



    assert(!(ldq_p(hpte) & HPTE_V_HVLOCK));

    args[0] = pte_index + i;

    return H_SUCCESS;

}