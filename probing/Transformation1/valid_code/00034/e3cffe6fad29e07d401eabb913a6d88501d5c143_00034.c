static inline void do_rfi(CPUPPCState *env, target_ulong nip, target_ulong msr)

{

    CPUState *cs = CPU(ppc_env_get_cpu(env));



    

    msr &= ~(1ULL << MSR_POW);



#if defined(TARGET_PPC64)

    

    if (!msr_is_64bit(env, msr)) {

        nip = (uint32_t)nip;

    }

#else

    nip = (uint32_t)nip;

#endif

    

    env->nip = nip & ~((target_ulong)0x00000003);

    hreg_store_msr(env, msr, 1);

#if defined(DEBUG_OP)

    cpu_dump_rfi(env->nip, env->msr);

#endif

    

    cs->interrupt_request |= CPU_INTERRUPT_EXITTB;



    

    check_tlb_flush(env);

}