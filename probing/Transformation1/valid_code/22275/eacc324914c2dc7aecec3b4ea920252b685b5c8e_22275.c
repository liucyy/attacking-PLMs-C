void ppc_slb_invalidate_all (CPUPPCState *env)

{

    

    tlb_flush(env, 1);

}