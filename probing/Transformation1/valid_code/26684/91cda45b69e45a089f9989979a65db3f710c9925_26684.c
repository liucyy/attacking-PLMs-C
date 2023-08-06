static int get_segment32(CPUPPCState *env, struct mmu_ctx_hash32 *ctx,

                         target_ulong eaddr, int rw, int type)

{

    hwaddr hash;

    target_ulong vsid;

    int ds, pr, target_page_bits;

    int ret, ret2;

    target_ulong sr, pgidx;



    pr = msr_pr;

    ctx->eaddr = eaddr;



    sr = env->sr[eaddr >> 28];

    ctx->key = (((sr & SR32_KP) && (pr != 0)) ||

                ((sr & SR32_KS) && (pr == 0))) ? 1 : 0;

    ds = !!(sr & SR32_T);

    ctx->nx = !!(sr & SR32_NX);

    vsid = sr & SR32_VSID;

    target_page_bits = TARGET_PAGE_BITS;

    LOG_MMU("Check segment v=" TARGET_FMT_lx " %d " TARGET_FMT_lx " nip="

            TARGET_FMT_lx " lr=" TARGET_FMT_lx

            " ir=%d dr=%d pr=%d %d t=%d\n",

            eaddr, (int)(eaddr >> 28), sr, env->nip, env->lr, (int)msr_ir,

            (int)msr_dr, pr != 0 ? 1 : 0, rw, type);

    pgidx = (eaddr & ~SEGMENT_MASK_256M) >> target_page_bits;

    hash = vsid ^ pgidx;

    ctx->ptem = (vsid << 7) | (pgidx >> 10);



    LOG_MMU("pte segment: key=%d ds %d nx %d vsid " TARGET_FMT_lx "\n",

            ctx->key, ds, ctx->nx, vsid);

    ret = -1;

    if (!ds) {

        

        if (type != ACCESS_CODE || ctx->nx == 0) {

            

            LOG_MMU("htab_base " TARGET_FMT_plx " htab_mask " TARGET_FMT_plx

                    " hash " TARGET_FMT_plx "\n",

                    env->htab_base, env->htab_mask, hash);

            ctx->hash[0] = hash;

            ctx->hash[1] = ~hash;



            

            ctx->raddr = (hwaddr)-1ULL;

            LOG_MMU("0 htab=" TARGET_FMT_plx "/" TARGET_FMT_plx

                    " vsid=" TARGET_FMT_lx " ptem=" TARGET_FMT_lx

                    " hash=" TARGET_FMT_plx "\n",

                    env->htab_base, env->htab_mask, vsid, ctx->ptem,

                    ctx->hash[0]);

            

            ret = find_pte32(env, ctx, 0, rw, type, target_page_bits);

            if (ret < 0) {

                

                LOG_MMU("1 htab=" TARGET_FMT_plx "/" TARGET_FMT_plx

                        " vsid=" TARGET_FMT_lx " api=" TARGET_FMT_lx

                        " hash=" TARGET_FMT_plx "\n", env->htab_base,

                        env->htab_mask, vsid, ctx->ptem, ctx->hash[1]);

                ret2 = find_pte32(env, ctx, 1, rw, type,

                                  target_page_bits);

                if (ret2 != -1) {

                    ret = ret2;

                }

            }

#if defined(DUMP_PAGE_TABLES)

            if (qemu_log_enabled()) {

                hwaddr curaddr;

                uint32_t a0, a1, a2, a3;



                qemu_log("Page table: " TARGET_FMT_plx " len " TARGET_FMT_plx

                         "\n", sdr, mask + 0x80);

                for (curaddr = sdr; curaddr < (sdr + mask + 0x80);

                     curaddr += 16) {

                    a0 = ldl_phys(curaddr);

                    a1 = ldl_phys(curaddr + 4);

                    a2 = ldl_phys(curaddr + 8);

                    a3 = ldl_phys(curaddr + 12);

                    if (a0 != 0 || a1 != 0 || a2 != 0 || a3 != 0) {

                        qemu_log(TARGET_FMT_plx ": %08x %08x %08x %08x\n",

                                 curaddr, a0, a1, a2, a3);

                    }

                }

            }

#endif

        } else {

            LOG_MMU("No access allowed\n");

            ret = -3;

        }

    } else {

        target_ulong sr;



        LOG_MMU("direct store...\n");

        



        

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

            qemu_log("ERROR: instruction should not need "

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