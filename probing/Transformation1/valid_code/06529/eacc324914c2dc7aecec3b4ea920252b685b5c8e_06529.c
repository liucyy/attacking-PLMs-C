void ppc_slb_invalidate_one (CPUPPCState *env, uint64_t T0)

{

    

    tlb_flush(env, 1);

}