static inline int get_segment_6xx_tlb(CPUPPCState *env, mmu_ctx_t *ctx,

                                      target_ulong eaddr, int rw, int type)

{

    hwaddr hash;

    target_ulong vsid;

    int ds, pr, target_page_bits;

    int ret;

    target_ulong sr, pgidx;



    pr = msr_pr;

    ctx->eaddr = eaddr;



    sr = env->sr[eaddr >> 28];

    ctx->key = (((sr & 0x20000000) && (pr != 0)) ||

                ((sr & 0x40000000) && (pr == 0))) ? 1 : 0;

    ds = sr & 0x80000000 ? 1 : 0;

    ctx->nx = sr & 0x10000000 ? 1 : 0;

    vsid = sr & 0x00FFFFFF;

    target_page_bits = TARGET_PAGE_BITS;

    qemu_log_mask(CPU_LOG_MMU,

            "Check segment v=" TARGET_FMT_lx " %d " TARGET_FMT_lx

            " nip=" TARGET_FMT_lx " lr=" TARGET_FMT_lx

            " ir=%d dr=%d pr=%d %d t=%d\n",

            eaddr, (int)(eaddr >> 28), sr, env->nip, env->lr, (int)msr_ir,

            (int)msr_dr, pr != 0 ? 1 : 0, rw, type);

    pgidx = (eaddr & ~SEGMENT_MASK_256M) >> target_page_bits;

    hash = vsid ^ pgidx;

    ctx->ptem = (vsid << 7) | (pgidx >> 10);



    qemu_log_mask(CPU_LOG_MMU,

            "pte segment: key=%d ds %d nx %d vsid " TARGET_FMT_lx "\n",

            ctx->key, ds, ctx->nx, vsid);

    ret = -1;

    if (!ds) {

        

        if (type != ACCESS_CODE || ctx->nx == 0) {

            

            qemu_log_mask(CPU_LOG_MMU, "htab_base " TARGET_FMT_plx

                    " htab_mask " TARGET_FMT_plx

                    " hash " TARGET_FMT_plx "\n",

                    env->htab_base, env->htab_mask, hash);

            ctx->hash[0] = hash;

            ctx->hash[1] = ~hash;



            

            ctx->raddr = (hwaddr)-1ULL;

            

            ret = ppc6xx_tlb_check(env, ctx, eaddr, rw, type);

#if defined(DUMP_PAGE_TABLES)

            if (qemu_loglevel_mask(CPU_LOG_MMU)) {

                CPUState *cs = ENV_GET_CPU(env);

                hwaddr curaddr;

                uint32_t a0, a1, a2, a3;



                qemu_log("Page table: " TARGET_FMT_plx " len " TARGET_FMT_plx

                         "\n", env->htab_base, env->htab_mask + 0x80);

                for (curaddr = env->htab_base;

                     curaddr < (env->htab_base + env->htab_mask + 0x80);

                     curaddr += 16) {

                    a0 = ldl_phys(cs->as, curaddr);

                    a1 = ldl_phys(cs->as, curaddr + 4);

                    a2 = ldl_phys(cs->as, curaddr + 8);

                    a3 = ldl_phys(cs->as, curaddr + 12);

                    if (a0 != 0 || a1 != 0 || a2 != 0 || a3 != 0) {

                        qemu_log(TARGET_FMT_plx ": %08x %08x %08x %08x\n",

                                 curaddr, a0, a1, a2, a3);

                    }

                }

            }

#endif

        } else {

            qemu_log_mask(CPU_LOG_MMU, "No access allowed\n");

            ret = -3;

        }

    } else {

        target_ulong sr;



        qemu_log_mask(CPU_LOG_MMU, "direct store...\n");

        



        

        sr = env->sr[eaddr >> 28];

        if ((sr & 0x1FF00000) >> 20 == 0x07f) {

            

            

            ctx->raddr = ((sr & 0xF) << 28) | (eaddr & 0x0FFFFFFF);

            ctx->prot = PAGE_READ | PAGE_WRITE | PAGE_EXEC;

            return 0;

        }



        switch (type) {

        case ACCESS_INT:

            

            break;

        case ACCESS_CODE:

            

            return -4;

        case ACCESS_FLOAT:

            

            return -4;

        case ACCESS_RES:

            

            return -4;

        case ACCESS_CACHE:

            

            

            ctx->raddr = eaddr;

            return 0;

        case ACCESS_EXT:

            

            return -4;

        default:

            qemu_log_mask(CPU_LOG_MMU, "ERROR: instruction should not need "

                          "address translation\n");

            return -4;

        }

        if ((rw == 1 || ctx->key != 1) && (rw == 0 || ctx->key != 0)) {

            ctx->raddr = eaddr;

            ret = 2;

        } else {

            ret = -2;

        }

    }



    return ret;

}