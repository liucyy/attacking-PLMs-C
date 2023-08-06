void ppc_store_sr (CPUPPCState *env, int srnum, target_ulong value)

{

    LOG_MMU("%s: reg=%d " TARGET_FMT_lx " " TARGET_FMT_lx "\n", __func__,

            srnum, value, env->sr[srnum]);

#if defined(TARGET_PPC64)

    if (env->mmu_model & POWERPC_MMU_64) {

        uint64_t rb = 0, rs = 0;



        

        rb |= ((uint32_t)srnum & 0xf) << 28;

        

        rb |= 1 << 27;

        

        rb |= (uint32_t)srnum;



        

        rs |= (value & 0xfffffff) << 12;

        

        rs |= ((value >> 27) & 0xf) << 9;



        ppc_store_slb(env, rb, rs);

    } else

#endif

    if (env->sr[srnum] != value) {

        env->sr[srnum] = value;



#if !defined(FLUSH_ALL_TLBS) && 0

        {

            target_ulong page, end;

            

            page = (16 << 20) * srnum;

            end = page + (16 << 20);

            for (; page != end; page += TARGET_PAGE_SIZE)

                tlb_flush_page(env, page);

        }

#else

        tlb_flush(env, 1);

#endif

    }

}