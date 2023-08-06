void helper_booke206_tlbwe(CPUPPCState *env)

{

    PowerPCCPU *cpu = ppc_env_get_cpu(env);

    uint32_t tlbncfg, tlbn;

    ppcmas_tlb_t *tlb;

    uint32_t size_tlb, size_ps;

    target_ulong mask;





    switch (env->spr[SPR_BOOKE_MAS0] & MAS0_WQ_MASK) {

    case MAS0_WQ_ALWAYS:

        

        break;

    case MAS0_WQ_COND:

        

        if (0) {

            return;

        }

        break;

    case MAS0_WQ_CLR_RSRV:

        

        return;

    default:

        

        return;

    }



    if (((env->spr[SPR_BOOKE_MAS0] & MAS0_ATSEL) == MAS0_ATSEL_LRAT) &&

        !msr_gs) {

        

        fprintf(stderr, "cpu: don't support LRAT setting yet\n");

        return;

    }



    tlbn = (env->spr[SPR_BOOKE_MAS0] & MAS0_TLBSEL_MASK) >> MAS0_TLBSEL_SHIFT;

    tlbncfg = env->spr[SPR_BOOKE_TLB0CFG + tlbn];



    tlb = booke206_cur_tlb(env);



    if (!tlb) {

        raise_exception_err_ra(env, POWERPC_EXCP_PROGRAM,

                               POWERPC_EXCP_INVAL |

                               POWERPC_EXCP_INVAL_INVAL, GETPC());

    }



    

    size_tlb = (env->spr[SPR_BOOKE_MAS1] & MAS1_TSIZE_MASK) >> MAS1_TSIZE_SHIFT;

    size_ps = booke206_tlbnps(env, tlbn);

    if ((env->spr[SPR_BOOKE_MAS1] & MAS1_VALID) && (tlbncfg & TLBnCFG_AVAIL) &&

        !(size_ps & (1 << size_tlb))) {

        raise_exception_err_ra(env, POWERPC_EXCP_PROGRAM,

                               POWERPC_EXCP_INVAL |

                               POWERPC_EXCP_INVAL_INVAL, GETPC());

    }



    if (msr_gs) {

        cpu_abort(CPU(cpu), "missing HV implementation\n");

    }

    tlb->mas7_3 = ((uint64_t)env->spr[SPR_BOOKE_MAS7] << 32) |

        env->spr[SPR_BOOKE_MAS3];

    tlb->mas1 = env->spr[SPR_BOOKE_MAS1];



    if ((env->spr[SPR_MMUCFG] & MMUCFG_MAVN) == MMUCFG_MAVN_V2) {

        

        booke206_fixed_size_tlbn(env, tlbn, tlb);

    } else {

        if (!(tlbncfg & TLBnCFG_AVAIL)) {

            

            tlb->mas1 &= ~MAS1_TSIZE_MASK;

            

            tlb->mas1 |= (tlbncfg & TLBnCFG_MINSIZE) >> 12;

        }

    }



    

    mask = ~(booke206_tlb_to_page_size(env, tlb) - 1);

    

    mask |= MAS2_ACM | MAS2_VLE | MAS2_W | MAS2_I | MAS2_M | MAS2_G | MAS2_E;



    if (!msr_cm) {

        

        mask &= 0xffffffff;

    }



    tlb->mas2 = env->spr[SPR_BOOKE_MAS2] & mask;



    if (!(tlbncfg & TLBnCFG_IPROT)) {

        

        tlb->mas1 &= ~MAS1_IPROT;

    }



    if (booke206_tlb_to_page_size(env, tlb) == TARGET_PAGE_SIZE) {

        tlb_flush_page(CPU(cpu), tlb->mas2 & MAS2_EPN_MASK);

    } else {

        tlb_flush(CPU(cpu));

    }

}