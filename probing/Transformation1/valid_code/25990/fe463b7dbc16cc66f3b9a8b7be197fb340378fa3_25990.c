void cpu_ppc_reset (void *opaque)

{

    CPUPPCState *env;

    target_ulong msr;



    env = opaque;

    msr = (target_ulong)0;

    if (0) {

        

        msr |= (target_ulong)MSR_HVB;

    }

    msr |= (target_ulong)0 << MSR_AP; 

    msr |= (target_ulong)0 << MSR_SA; 

    msr |= (target_ulong)1 << MSR_EP;

#if defined (DO_SINGLE_STEP) && 0

    

    msr |= (target_ulong)1 << MSR_SE;

    msr |= (target_ulong)1 << MSR_BE;

#endif

#if defined(CONFIG_USER_ONLY)

    msr |= (target_ulong)1 << MSR_FP; 

    msr |= (target_ulong)1 << MSR_PR;

#else

    env->nip = env->hreset_vector | env->excp_prefix;

    if (env->mmu_model != POWERPC_MMU_REAL)

        ppc_tlb_invalidate_all(env);

#endif

    env->msr = msr;

    hreg_compute_hflags(env);

    env->reserve = (target_ulong)-1ULL;

    

    env->pending_interrupts = 0;

    env->exception_index = POWERPC_EXCP_NONE;

    env->error_code = 0;

    

    tlb_flush(env, 1);

}