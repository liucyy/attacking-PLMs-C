POWERPC_FAMILY(POWER8E)(ObjectClass *oc, void *data)

{

    DeviceClass *dc = DEVICE_CLASS(oc);

    PowerPCCPUClass *pcc = POWERPC_CPU_CLASS(oc);



    dc->fw_name = "PowerPC,POWER8";

    dc->desc = "POWER8E";

    dc->props = powerpc_servercpu_properties;

    pcc->pvr_match = ppc_pvr_match_power8;

    pcc->pcr_mask = PCR_COMPAT_2_05 | PCR_COMPAT_2_06;

    pcc->init_proc = init_proc_POWER8;

    pcc->check_pow = check_pow_nocheck;

    pcc->insns_flags = PPC_INSNS_BASE | PPC_ISEL | PPC_STRING | PPC_MFTB |

                       PPC_FLOAT | PPC_FLOAT_FSEL | PPC_FLOAT_FRES |

                       PPC_FLOAT_FSQRT | PPC_FLOAT_FRSQRTE |

                       PPC_FLOAT_FRSQRTES |

                       PPC_FLOAT_STFIWX |

                       PPC_FLOAT_EXT |

                       PPC_CACHE | PPC_CACHE_ICBI | PPC_CACHE_DCBZ |

                       PPC_MEM_SYNC | PPC_MEM_EIEIO |

                       PPC_MEM_TLBIE | PPC_MEM_TLBSYNC |

                       PPC_64B | PPC_64BX | PPC_ALTIVEC |

                       PPC_SEGMENT_64B | PPC_SLBI |

                       PPC_POPCNTB | PPC_POPCNTWD;

    pcc->insns_flags2 = PPC2_VSX | PPC2_VSX207 | PPC2_DFP | PPC2_DBRX |

                        PPC2_PERM_ISA206 | PPC2_DIVE_ISA206 |

                        PPC2_ATOMIC_ISA206 | PPC2_FP_CVT_ISA206 |

                        PPC2_FP_TST_ISA206 | PPC2_BCTAR_ISA207 |

                        PPC2_LSQ_ISA207 | PPC2_ALTIVEC_207 |

                        PPC2_ISA205 | PPC2_ISA207S;

    pcc->msr_mask = (1ull << MSR_SF) |

                    (1ull << MSR_TM) |

                    (1ull << MSR_VR) |

                    (1ull << MSR_VSX) |

                    (1ull << MSR_EE) |

                    (1ull << MSR_PR) |

                    (1ull << MSR_FP) |

                    (1ull << MSR_ME) |

                    (1ull << MSR_FE0) |

                    (1ull << MSR_SE) |

                    (1ull << MSR_DE) |

                    (1ull << MSR_FE1) |

                    (1ull << MSR_IR) |

                    (1ull << MSR_DR) |

                    (1ull << MSR_PMM) |

                    (1ull << MSR_RI) |

                    (1ull << MSR_LE);

    pcc->mmu_model = POWERPC_MMU_2_06;

#if defined(CONFIG_SOFTMMU)

    pcc->handle_mmu_fault = ppc_hash64_handle_mmu_fault;

#endif

    pcc->excp_model = POWERPC_EXCP_POWER7;

    pcc->bus_model = PPC_FLAGS_INPUT_POWER7;

    pcc->bfd_mach = bfd_mach_ppc64;

    pcc->flags = POWERPC_FLAG_VRE | POWERPC_FLAG_SE |

                 POWERPC_FLAG_BE | POWERPC_FLAG_PMM |

                 POWERPC_FLAG_BUS_CLK | POWERPC_FLAG_CFAR |

                 POWERPC_FLAG_VSX;

    pcc->l1_dcache_size = 0x8000;

    pcc->l1_icache_size = 0x8000;

    pcc->interrupts_big_endian = ppc_cpu_interrupts_big_endian_lpcr;

}