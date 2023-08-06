static hwaddr ppc_hash64_htab_lookup(PowerPCCPU *cpu,

                                     ppc_slb_t *slb, target_ulong eaddr,

                                     ppc_hash_pte64_t *pte, unsigned *pshift)

{

    CPUPPCState *env = &cpu->env;

    hwaddr hash, ptex;

    uint64_t vsid, epnmask, epn, ptem;

    const struct ppc_one_seg_page_size *sps = slb->sps;



    

    assert(sps);



    

    if (env->spr[SPR_LPCR] & LPCR_ISL) {

        

        sps = &env->sps.sps[0];

        assert(sps->page_shift == 12);

    }



    epnmask = ~((1ULL << sps->page_shift) - 1);



    if (slb->vsid & SLB_VSID_B) {

        

        vsid = (slb->vsid & SLB_VSID_VSID) >> SLB_VSID_SHIFT_1T;

        epn = (eaddr & ~SEGMENT_MASK_1T) & epnmask;

        hash = vsid ^ (vsid << 25) ^ (epn >> sps->page_shift);

    } else {

        

        vsid = (slb->vsid & SLB_VSID_VSID) >> SLB_VSID_SHIFT;

        epn = (eaddr & ~SEGMENT_MASK_256M) & epnmask;

        hash = vsid ^ (epn >> sps->page_shift);

    }

    ptem = (slb->vsid & SLB_VSID_PTEM) | ((epn >> 16) & HPTE64_V_AVPN);

    ptem |= HPTE64_V_VALID;



    

    qemu_log_mask(CPU_LOG_MMU,

            "htab_base " TARGET_FMT_plx " htab_mask " TARGET_FMT_plx

            " hash " TARGET_FMT_plx "\n",

            env->htab_base, env->htab_mask, hash);



    

    qemu_log_mask(CPU_LOG_MMU,

            "0 htab=" TARGET_FMT_plx "/" TARGET_FMT_plx

            " vsid=" TARGET_FMT_lx " ptem=" TARGET_FMT_lx

            " hash=" TARGET_FMT_plx "\n",

            env->htab_base, env->htab_mask, vsid, ptem,  hash);

    ptex = ppc_hash64_pteg_search(cpu, hash, sps, ptem, pte, pshift);



    if (ptex == -1) {

        

        ptem |= HPTE64_V_SECONDARY;

        qemu_log_mask(CPU_LOG_MMU,

                "1 htab=" TARGET_FMT_plx "/" TARGET_FMT_plx

                " vsid=" TARGET_FMT_lx " api=" TARGET_FMT_lx

                " hash=" TARGET_FMT_plx "\n", env->htab_base,

                env->htab_mask, vsid, ptem, ~hash);



        ptex = ppc_hash64_pteg_search(cpu, ~hash, sps, ptem, pte, pshift);

    }



    return ptex;

}