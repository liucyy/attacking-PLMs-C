uint32_t cpu_mips_get_random (CPUMIPSState *env)

{

    static uint32_t seed = 1;

    static uint32_t prev_idx = 0;

    uint32_t idx;

    

    do {

        

        seed = 1103515245 * seed + 12345;

        idx = (seed >> 16) % (env->tlb->nb_tlb - env->CP0_Wired) +

              env->CP0_Wired;

    } while (idx == prev_idx);

    prev_idx = idx;

    return idx;

}