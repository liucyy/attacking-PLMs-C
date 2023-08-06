static void mvp_init (CPUMIPSState *env, const mips_def_t *def)

{

    env->mvp = qemu_mallocz(sizeof(CPUMIPSMVPContext));



    

    env->mvp->CP0_MVPConf0 = (1 << CP0MVPC0_M) | (1 << CP0MVPC0_TLBS) |

                             (0 << CP0MVPC0_GS) | (1 << CP0MVPC0_PCP) |







                             (1 << CP0MVPC0_TCA) | (0x0 << CP0MVPC0_PVPE) |

                             (0x04 << CP0MVPC0_PTC);

    

    if (!env->user_mode_only)

        env->mvp->CP0_MVPConf0 |= (env->tlb->nb_tlb << CP0MVPC0_PTLBE);



    

    env->mvp->CP0_MVPConf1 = (1 << CP0MVPC1_CIM) | (1 << CP0MVPC1_CIF) |

                             (0x0 << CP0MVPC1_PCX) | (0x0 << CP0MVPC1_PCP2) |

                             (0x1 << CP0MVPC1_PCP1);

}