static inline bool valid_ptex(PowerPCCPU *cpu, target_ulong ptex)

{

    

    if (((ptex & ~7ULL) / HPTES_PER_GROUP) & ~cpu->env.htab_mask) {

        return false;

    }

    return true;

}