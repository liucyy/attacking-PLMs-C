int mmu_translate_real(CPUS390XState *env, target_ulong raddr, int rw,

                       target_ulong *addr, int *flags)

{

    

    *flags = PAGE_READ | PAGE_WRITE;

    *addr = mmu_real2abs(env, raddr);



    

    return 0;

}