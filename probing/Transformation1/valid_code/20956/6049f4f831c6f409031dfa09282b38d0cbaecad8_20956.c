CPUAlphaState * cpu_alpha_init (const char *cpu_model)

{

    CPUAlphaState *env;

    int implver, amask, i, max;



    env = qemu_mallocz(sizeof(CPUAlphaState));

    cpu_exec_init(env);

    alpha_translate_init();

    tlb_flush(env, 1);



    

    implver = IMPLVER_21264;

    amask = (AMASK_BWX | AMASK_FIX | AMASK_CIX | AMASK_MVI

	     | AMASK_TRAP | AMASK_PREFETCH);



    max = ARRAY_SIZE(cpu_defs);

    for (i = 0; i < max; i++) {

        if (strcmp (cpu_model, cpu_defs[i].name) == 0) {

            implver = cpu_defs[i].implver;

            amask = cpu_defs[i].amask;

            break;

        }

    }

    env->implver = implver;

    env->amask = amask;



    env->ps = 0x1F00;

#if defined (CONFIG_USER_ONLY)

    env->ps |= 1 << 3;

    cpu_alpha_store_fpcr(env, (FPCR_INVD | FPCR_DZED | FPCR_OVFD

                               | FPCR_UNFD | FPCR_INED | FPCR_DNOD));

#endif

    pal_init(env);



    

#if defined (CONFIG_USER_ONLY)

    env->ipr[IPR_EXC_ADDR] = 0;

    env->ipr[IPR_EXC_SUM] = 0;

    env->ipr[IPR_EXC_MASK] = 0;

#else

    {

        uint64_t hwpcb;

        hwpcb = env->ipr[IPR_PCBB];

        env->ipr[IPR_ASN] = 0;

        env->ipr[IPR_ASTEN] = 0;

        env->ipr[IPR_ASTSR] = 0;

        env->ipr[IPR_DATFX] = 0;

        

        

        

        

        

        env->ipr[IPR_FEN] = 0;

        env->ipr[IPR_IPL] = 31;

        env->ipr[IPR_MCES] = 0;

        env->ipr[IPR_PERFMON] = 0; 

        

        env->ipr[IPR_SISR] = 0;

        env->ipr[IPR_VIRBND] = -1ULL;

    }

#endif



    qemu_init_vcpu(env);

    return env;

}