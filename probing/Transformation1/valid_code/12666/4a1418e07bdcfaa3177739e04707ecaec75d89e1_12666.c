static void kqemu_update_cpuid(CPUState *env)

{

    int critical_features_mask, features, ext_features, ext_features_mask;

    uint32_t eax, ebx, ecx, edx;



    

    critical_features_mask =

        CPUID_CMOV | CPUID_CX8 |

        CPUID_FXSR | CPUID_MMX | CPUID_SSE |

        CPUID_SSE2 | CPUID_SEP;

    ext_features_mask = CPUID_EXT_SSE3 | CPUID_EXT_MONITOR;

    if (!is_cpuid_supported()) {

        features = 0;

        ext_features = 0;

    } else {

        cpuid(1, eax, ebx, ecx, edx);

        features = edx;

        ext_features = ecx;

    }

#ifdef __x86_64__

    

    features &= ~CPUID_SEP;

#endif

    env->cpuid_features = (env->cpuid_features & ~critical_features_mask) |

        (features & critical_features_mask);

    env->cpuid_ext_features = (env->cpuid_ext_features & ~ext_features_mask) |

        (ext_features & ext_features_mask);

    

}