static hwaddr ppc_hash64_htab_lookup(CPUPPCState *env,

                                     ppc_slb_t *slb, target_ulong eaddr,

                                     ppc_hash_pte64_t *pte)

{

    hwaddr pteg_off, pte_offset;

    hwaddr hash;

    uint64_t vsid, epnshift, epnmask, epn, ptem;



    

    epnshift = (slb->vsid & SLB_VSID_L)

        ? TARGET_PAGE_BITS_16M : TARGET_PAGE_BITS;

    epnmask = ~((1ULL << epnshift) - 1);



    if (slb->vsid & SLB_VSID_B) {

        

        vsid = (slb->vsid & SLB_VSID_VSID) >> SLB_VSID_SHIFT_1T;

        epn = (eaddr & ~SEGMENT_MASK_1T) & epnmask;

        hash = vsid ^ (vsid << 25) ^ (epn >> epnshift);

    } else {

        

        vsid = (slb->vsid & SLB_VSID_VSID) >> SLB_VSID_SHIFT;

        epn = (eaddr & ~SEGMENT_MASK_256M) & epnmask;

        hash = vsid ^ (epn >> epnshift);

    }

    ptem = (slb->vsid & SLB_VSID_PTEM) | ((epn >> 16) & HPTE64_V_AVPN);



    

    LOG_MMU("htab_base " TARGET_FMT_plx " htab_mask " TARGET_FMT_plx

            " hash " TARGET_FMT_plx "\n",

            env->htab_base, env->htab_mask, hash);



    

    LOG_MMU("0 htab=" TARGET_FMT_plx "/" TARGET_FMT_plx

            " vsid=" TARGET_FMT_lx " ptem=" TARGET_FMT_lx

            " hash=" TARGET_FMT_plx "\n",

            env->htab_base, env->htab_mask, vsid, ptem,  hash);

    pteg_off = (hash * HASH_PTEG_SIZE_64) & env->htab_mask;

    pte_offset = ppc_hash64_pteg_search(env, pteg_off, 0, ptem, pte);



    if (pte_offset == -1) {

        

        LOG_MMU("1 htab=" TARGET_FMT_plx "/" TARGET_FMT_plx

                " vsid=" TARGET_FMT_lx " api=" TARGET_FMT_lx

                " hash=" TARGET_FMT_plx "\n", env->htab_base,

                env->htab_mask, vsid, ptem, ~hash);



        pteg_off = (~hash * HASH_PTEG_SIZE_64) & env->htab_mask;

        pte_offset = ppc_hash64_pteg_search(env, pteg_off, 1, ptem, pte);

    }



    return pte_offset;

}