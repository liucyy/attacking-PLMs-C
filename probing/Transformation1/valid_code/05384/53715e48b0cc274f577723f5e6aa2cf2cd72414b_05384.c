void r4k_invalidate_tlb (CPUState *env, int idx, int use_extra)

{

    r4k_tlb_t *tlb;

    target_ulong addr;

    target_ulong end;

    uint8_t ASID = env->CP0_EntryHi & 0xFF;

    target_ulong mask;



    tlb = &env->tlb->mmu.r4k.tlb[idx];

    

    if (tlb->G == 0 && tlb->ASID != ASID) {

        return;

    }



    if (use_extra && env->tlb->tlb_in_use < MIPS_TLB_MAX) {

        

        env->tlb->mmu.r4k.tlb[env->tlb->tlb_in_use] = *tlb;

        env->tlb->tlb_in_use++;

        return;

    }



    

    mask = tlb->PageMask | ~(TARGET_PAGE_MASK << 1);

    if (tlb->V0) {

        addr = tlb->VPN & ~mask;

#if defined(TARGET_MIPS64)

        if (addr >= (0xFFFFFFFF80000000ULL & env->SEGMask)) {

            addr |= 0x3FFFFF0000000000ULL;

        }

#endif

        end = addr | (mask >> 1);

        while (addr < end) {

            tlb_flush_page (env, addr);

            addr += TARGET_PAGE_SIZE;

        }

    }

    if (tlb->V1) {

        addr = (tlb->VPN & ~mask) | ((mask >> 1) + 1);

#if defined(TARGET_MIPS64)

        if (addr >= (0xFFFFFFFF80000000ULL & env->SEGMask)) {

            addr |= 0x3FFFFF0000000000ULL;

        }

#endif

        end = addr | mask;

        while (addr < end) {

            tlb_flush_page (env, addr);

            addr += TARGET_PAGE_SIZE;

        }

    }

}