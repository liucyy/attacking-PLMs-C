static void *atomic_mmu_lookup(CPUArchState *env, target_ulong addr,

                               TCGMemOpIdx oi, uintptr_t retaddr)

{

    size_t mmu_idx = get_mmuidx(oi);

    size_t index = (addr >> TARGET_PAGE_BITS) & (CPU_TLB_SIZE - 1);

    CPUTLBEntry *tlbe = &env->tlb_table[mmu_idx][index];

    target_ulong tlb_addr = tlbe->addr_write;

    TCGMemOp mop = get_memop(oi);

    int a_bits = get_alignment_bits(mop);

    int s_bits = mop & MO_SIZE;



    

    retaddr -= GETPC_ADJ;



    

    if (unlikely(a_bits > 0 && (addr & ((1 << a_bits) - 1)))) {

        

        cpu_unaligned_access(ENV_GET_CPU(env), addr, MMU_DATA_STORE,

                             mmu_idx, retaddr);

    }



    

    if (unlikely(addr & ((1 << s_bits) - 1))) {

        

        goto stop_the_world;

    }



    

    if ((addr & TARGET_PAGE_MASK)

        != (tlb_addr & (TARGET_PAGE_MASK | TLB_INVALID_MASK))) {

        if (!VICTIM_TLB_HIT(addr_write, addr)) {

            tlb_fill(ENV_GET_CPU(env), addr, MMU_DATA_STORE, mmu_idx, retaddr);

        }

        tlb_addr = tlbe->addr_write;

    }



    

    if (unlikely(tlb_addr & TLB_NOTDIRTY)) {

        tlb_set_dirty(ENV_GET_CPU(env), addr);

        tlb_addr = tlb_addr & ~TLB_NOTDIRTY;

    }



    

    if (unlikely(tlb_addr & ~TARGET_PAGE_MASK)) {

        

        goto stop_the_world;

    }



    

    if (unlikely(tlbe->addr_read != tlb_addr)) {

        tlb_fill(ENV_GET_CPU(env), addr, MMU_DATA_LOAD, mmu_idx, retaddr);

        

        goto stop_the_world;

    }



    return (void *)((uintptr_t)addr + tlbe->addend);



 stop_the_world:

    cpu_loop_exit_atomic(ENV_GET_CPU(env), retaddr);

}