static always_inline int _pte_check (mmu_ctx_t *ctx, int is_64b,

                                     target_ulong pte0, target_ulong pte1,

                                     int h, int rw, int type)

{

    target_ulong ptem, mmask;

    int access, ret, pteh, ptev, pp;



    access = 0;

    ret = -1;

    

#if defined(TARGET_PPC64)

    if (is_64b) {

        ptev = pte64_is_valid(pte0);

        pteh = (pte0 >> 1) & 1;

    } else

#endif

    {

        ptev = pte_is_valid(pte0);

        pteh = (pte0 >> 6) & 1;

    }

    if (ptev && h == pteh) {

        

#if defined(TARGET_PPC64)

        if (is_64b) {

            ptem = pte0 & PTE64_PTEM_MASK;

            mmask = PTE64_CHECK_MASK;

            pp = (pte1 & 0x00000003) | ((pte1 >> 61) & 0x00000004);

            ctx->nx |= (pte1 >> 2) & 1; 

            ctx->nx |= (pte1 >> 3) & 1; 

        } else

#endif

        {

            ptem = pte0 & PTE_PTEM_MASK;

            mmask = PTE_CHECK_MASK;

            pp = pte1 & 0x00000003;

        }

        if (ptem == ctx->ptem) {

            if (ctx->raddr != (target_ulong)-1) {

                

                if ((ctx->raddr & mmask) != (pte1 & mmask)) {

                    if (loglevel != 0)

                        fprintf(logfile, "Bad RPN/WIMG/PP\n");

                    return -3;

                }

            }

            

            access = pp_check(ctx->key, pp, ctx->nx);

            

            ctx->raddr = pte1;

            ctx->prot = access;

            ret = check_prot(ctx->prot, rw, type);

            if (ret == 0) {

                

#if defined (DEBUG_MMU)

                if (loglevel != 0)

                    fprintf(logfile, "PTE access granted !\n");

#endif

            } else {

                

#if defined (DEBUG_MMU)

                if (loglevel != 0)

                    fprintf(logfile, "PTE access rejected\n");

#endif

            }

        }

    }



    return ret;

}