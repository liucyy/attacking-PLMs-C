void helper_slbie(CPUPPCState *env, target_ulong addr)

{

    PowerPCCPU *cpu = ppc_env_get_cpu(env);

    ppc_slb_t *slb;



    slb = slb_lookup(cpu, addr);

    if (!slb) {

        return;

    }



    if (slb->esid & SLB_ESID_V) {

        slb->esid &= ~SLB_ESID_V;



        

        tlb_flush(CPU(cpu), 1);

    }

}