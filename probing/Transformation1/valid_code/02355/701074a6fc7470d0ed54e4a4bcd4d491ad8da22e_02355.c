static void raise_mmu_exception(CPUMIPSState *env, target_ulong address,

                                int rw, int tlb_error)

{

    CPUState *cs = CPU(mips_env_get_cpu(env));

    int exception = 0, error_code = 0;



    if (rw == MMU_INST_FETCH) {

        error_code |= EXCP_INST_NOTAVAIL;

    }



    switch (tlb_error) {

    default:

    case TLBRET_BADADDR:

        

        

        if (rw == MMU_DATA_STORE) {

            exception = EXCP_AdES;

        } else {

            exception = EXCP_AdEL;

        }

        break;

    case TLBRET_NOMATCH:

        

        if (rw == MMU_DATA_STORE) {

            exception = EXCP_TLBS;

        } else {

            exception = EXCP_TLBL;

        }

        error_code |= EXCP_TLB_NOMATCH;

        break;

    case TLBRET_INVALID:

        

        if (rw == MMU_DATA_STORE) {

            exception = EXCP_TLBS;

        } else {

            exception = EXCP_TLBL;

        }

        break;

    case TLBRET_DIRTY:

        

        exception = EXCP_LTLBL;

        break;

    case TLBRET_XI:

        

        if (env->CP0_PageGrain & (1 << CP0PG_IEC)) {

            exception = EXCP_TLBXI;

        } else {

            exception = EXCP_TLBL;

        }

        break;

    case TLBRET_RI:

        

        if (env->CP0_PageGrain & (1 << CP0PG_IEC)) {

            exception = EXCP_TLBRI;

        } else {

            exception = EXCP_TLBL;

        }

        break;

    }

    

    env->CP0_BadVAddr = address;

    env->CP0_Context = (env->CP0_Context & ~0x007fffff) |

                       ((address >> 9) & 0x007ffff0);

    env->CP0_EntryHi = (env->CP0_EntryHi & env->CP0_EntryHi_ASID_mask) |


                       (address & (TARGET_PAGE_MASK << 1));

#if defined(TARGET_MIPS64)

    env->CP0_EntryHi &= env->SEGMask;

    env->CP0_XContext =

           (env->CP0_XContext & ((~0ULL) << (env->SEGBITS - 7))) |

                 (extract64(address, 62, 2) << (env->SEGBITS - 9)) |

           (extract64(address, 13, env->SEGBITS - 13) << 4);

#endif

    cs->exception_index = exception;

    env->error_code = error_code;

}