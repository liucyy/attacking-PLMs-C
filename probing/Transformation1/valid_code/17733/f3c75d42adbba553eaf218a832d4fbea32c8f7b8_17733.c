static target_ulong h_enter(PowerPCCPU *cpu, sPAPREnvironment *spapr,

                            target_ulong opcode, target_ulong *args)

{

    CPUPPCState *env = &cpu->env;

    target_ulong flags = args[0];

    target_ulong pte_index = args[1];

    target_ulong pteh = args[2];

    target_ulong ptel = args[3];

    target_ulong page_shift = 12;

    target_ulong raddr;

    target_ulong i;

    hwaddr hpte;



    

    if (pteh & HPTE64_V_LARGE) {

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



    raddr = (ptel & HPTE64_R_RPN) & ~((1ULL << page_shift) - 1);



    if (raddr < spapr->ram_limit) {

        

        if ((ptel & HPTE64_R_WIMG) != HPTE64_R_M) {

            return H_PARAMETER;

        }

    } else {

        

        

        

        if ((ptel & (HPTE64_R_W | HPTE64_R_I | HPTE64_R_M)) != HPTE64_R_I) {

            return H_PARAMETER;

        }

    }



    pteh &= ~0x60ULL;



    if ((pte_index * HASH_PTE_SIZE_64) & ~env->htab_mask) {

        return H_PARAMETER;

    }

    if (likely((flags & H_EXACT) == 0)) {

        pte_index &= ~7ULL;

        hpte = pte_index * HASH_PTE_SIZE_64;

        for (i = 0; ; ++i) {

            if (i == 8) {

                return H_PTEG_FULL;

            }

            if ((ppc_hash64_load_hpte0(env, hpte) & HPTE64_V_VALID) == 0) {

                break;

            }

            hpte += HASH_PTE_SIZE_64;

        }

    } else {

        i = 0;

        hpte = pte_index * HASH_PTE_SIZE_64;

        if (ppc_hash64_load_hpte0(env, hpte) & HPTE64_V_VALID) {

            return H_PTEG_FULL;

        }

    }

    ppc_hash64_store_hpte1(env, hpte, ptel);

    

    ppc_hash64_store_hpte0(env, hpte, pteh | HPTE64_V_HPTE_DIRTY);



    args[0] = pte_index + i;

    return H_SUCCESS;

}