static void invalidate_tlb (int idx, int use_extra)

{

    tlb_t *tlb;

    target_ulong addr;

    uint8_t ASID;



    ASID = env->CP0_EntryHi & 0xFF;



    tlb = &env->tlb[idx];

    

    if (tlb->G == 0 && tlb->ASID != ASID) {

        return;

    }



    if (use_extra && env->tlb_in_use < MIPS_TLB_MAX) {

        

        env->tlb[env->tlb_in_use] = *tlb;

        env->tlb_in_use++;

        return;

    }



    if (tlb->V0) {

        tb_invalidate_page_range(tlb->PFN[0], tlb->end - tlb->VPN);

        addr = tlb->VPN;

        while (addr < tlb->end) {

            tlb_flush_page (env, addr);

            addr += TARGET_PAGE_SIZE;

        }

    }

    if (tlb->V1) {

        tb_invalidate_page_range(tlb->PFN[1], tlb->end2 - tlb->end);

        addr = tlb->end;

        while (addr < tlb->end2) {

            tlb_flush_page (env, addr);

            addr += TARGET_PAGE_SIZE;

        }

    }

}