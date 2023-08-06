void cpu_x86_update_cr0(CPUX86State *env)

{

    int pg_state, pe_state;



#if defined(DEBUG_MMU)

    printf("CR0 update: CR0=0x%08x\n", env->cr[0]);

#endif

    pg_state = env->cr[0] & CR0_PG_MASK;

    if (pg_state != last_pg_state) {

        tlb_flush(env);

        last_pg_state = pg_state;

    }

    

    pe_state = (env->cr[0] & CR0_PE_MASK);

    env->hflags = (env->hflags & ~HF_PE_MASK) | (pe_state << HF_PE_SHIFT);

    

    env->hflags |= ((pe_state ^ 1) << HF_ADDSEG_SHIFT);

}