int ppc_hash64_handle_mmu_fault(PowerPCCPU *cpu, vaddr eaddr,

                                int rwx, int mmu_idx)

{

    CPUState *cs = CPU(cpu);

    CPUPPCState *env = &cpu->env;

    ppc_slb_t *slb;

    unsigned apshift;

    hwaddr pte_offset;

    ppc_hash_pte64_t pte;

    int pp_prot, amr_prot, prot;

    uint64_t new_pte1, dsisr;

    const int need_prot[] = {PAGE_READ, PAGE_WRITE, PAGE_EXEC};

    hwaddr raddr;



    assert((rwx == 0) || (rwx == 1) || (rwx == 2));



    

    if (((rwx == 2) && (msr_ir == 0)) || ((rwx != 2) && (msr_dr == 0))) {

        

        

        raddr = eaddr & 0x0FFFFFFFFFFFFFFFULL;

        tlb_set_page(cs, eaddr & TARGET_PAGE_MASK, raddr & TARGET_PAGE_MASK,

                     PAGE_READ | PAGE_WRITE | PAGE_EXEC, mmu_idx,

                     TARGET_PAGE_SIZE);

        return 0;

    }



    

    slb = slb_lookup(cpu, eaddr);



    if (!slb) {

        if (rwx == 2) {

            cs->exception_index = POWERPC_EXCP_ISEG;

            env->error_code = 0;

        } else {

            cs->exception_index = POWERPC_EXCP_DSEG;

            env->error_code = 0;

            env->spr[SPR_DAR] = eaddr;

        }

        return 1;

    }



    

    if ((rwx == 2) && (slb->vsid & SLB_VSID_N)) {

        ppc_hash64_set_isi(cs, env, 0x10000000);

        return 1;

    }



    

    pte_offset = ppc_hash64_htab_lookup(cpu, slb, eaddr, &pte);

    if (pte_offset == -1) {

        dsisr = 0x40000000;

        if (rwx == 2) {

            ppc_hash64_set_isi(cs, env, dsisr);

        } else {

            if (rwx == 1) {

                dsisr |= 0x02000000;

            }

            ppc_hash64_set_dsi(cs, env, eaddr, dsisr);

        }

        return 1;

    }

    qemu_log_mask(CPU_LOG_MMU,

                "found PTE at offset %08" HWADDR_PRIx "\n", pte_offset);



    

    apshift = hpte_page_shift(slb->sps, pte.pte0, pte.pte1);

    if (!apshift) {

        error_report("Bad page size encoding in HPTE 0x%"PRIx64" - 0x%"PRIx64

                     " @ 0x%"HWADDR_PRIx, pte.pte0, pte.pte1, pte_offset);

        

        cs->exception_index = POWERPC_EXCP_MCHECK;

        env->error_code = 0;

        return 1;

    }



    



    pp_prot = ppc_hash64_pte_prot(cpu, slb, pte);

    amr_prot = ppc_hash64_amr_prot(cpu, pte);

    prot = pp_prot & amr_prot;



    if ((need_prot[rwx] & ~prot) != 0) {

        

        qemu_log_mask(CPU_LOG_MMU, "PTE access rejected\n");

        if (rwx == 2) {

            ppc_hash64_set_isi(cs, env, 0x08000000);

        } else {

            dsisr = 0;

            if (need_prot[rwx] & ~pp_prot) {

                dsisr |= 0x08000000;

            }

            if (rwx == 1) {

                dsisr |= 0x02000000;

            }

            if (need_prot[rwx] & ~amr_prot) {

                dsisr |= 0x00200000;

            }

            ppc_hash64_set_dsi(cs, env, eaddr, dsisr);

        }

        return 1;

    }



    qemu_log_mask(CPU_LOG_MMU, "PTE access granted !\n");



    



    new_pte1 = pte.pte1 | HPTE64_R_R; 

    if (rwx == 1) {

        new_pte1 |= HPTE64_R_C; 

    } else {

        

        prot &= ~PAGE_WRITE;

    }



    if (new_pte1 != pte.pte1) {

        ppc_hash64_store_hpte(cpu, pte_offset / HASH_PTE_SIZE_64,

                              pte.pte0, new_pte1);

    }



    



    raddr = deposit64(pte.pte1 & HPTE64_R_RPN, 0, apshift, eaddr);



    tlb_set_page(cs, eaddr & TARGET_PAGE_MASK, raddr & TARGET_PAGE_MASK,

                 prot, mmu_idx, 1ULL << apshift);



    return 0;

}