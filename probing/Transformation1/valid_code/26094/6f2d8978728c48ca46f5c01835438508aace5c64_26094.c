int mmu40x_get_physical_address (CPUState *env, mmu_ctx_t *ctx,

                                 target_ulong address, int rw, int access_type)

{

    ppcemb_tlb_t *tlb;

    target_phys_addr_t raddr;

    int i, ret, zsel, zpr, pr;



    ret = -1;

    raddr = -1;

    pr = msr_pr;

    for (i = 0; i < env->nb_tlb; i++) {

        tlb = &env->tlb[i].tlbe;

        if (ppcemb_tlb_check(env, tlb, &raddr, address,

                             env->spr[SPR_40x_PID], 0, i) < 0)

            continue;

        zsel = (tlb->attr >> 4) & 0xF;

        zpr = (env->spr[SPR_40x_ZPR] >> (28 - (2 * zsel))) & 0x3;

#if defined (DEBUG_SOFTWARE_TLB)

        if (loglevel != 0) {

            fprintf(logfile, "%s: TLB %d zsel %d zpr %d rw %d attr %08x\n",

                    __func__, i, zsel, zpr, rw, tlb->attr);

        }

#endif

        

        switch (zpr) {

        case 0x2:

            if (pr != 0)

                goto check_perms;

            

        case 0x3:

            

            ctx->prot = PAGE_READ | PAGE_WRITE | PAGE_EXEC;

            ret = 0;

            break;

        case 0x0:

            if (pr != 0) {

                ctx->prot = 0;

                ret = -2;

                break;

            }

            

        case 0x1:

        check_perms:

            

            

            ctx->prot = tlb->prot;

            ctx->prot |= PAGE_EXEC;

            ret = check_prot(ctx->prot, rw, access_type);

            break;

        }

        if (ret >= 0) {

            ctx->raddr = raddr;

#if defined (DEBUG_SOFTWARE_TLB)

            if (loglevel != 0) {

                fprintf(logfile, "%s: access granted " ADDRX " => " REGX

                        " %d %d\n", __func__, address, ctx->raddr, ctx->prot,

                        ret);

            }

#endif

            return 0;

        }

    }

#if defined (DEBUG_SOFTWARE_TLB)

    if (loglevel != 0) {

        fprintf(logfile, "%s: access refused " ADDRX " => " REGX

                " %d %d\n", __func__, address, raddr, ctx->prot,

                ret);

    }

#endif



    return ret;

}