void arm_cpu_do_transaction_failed(CPUState *cs, hwaddr physaddr,

                                   vaddr addr, unsigned size,

                                   MMUAccessType access_type,

                                   int mmu_idx, MemTxAttrs attrs,

                                   MemTxResult response, uintptr_t retaddr)

{

    ARMCPU *cpu = ARM_CPU(cs);

    ARMMMUFaultInfo fi = {};



    

    cpu_restore_state(cs, retaddr);



    

    fi.ea = (response != MEMTX_DECODE_ERROR);

    fi.type = ARMFault_SyncExternal;

    deliver_fault(cpu, addr, access_type, mmu_idx, &fi);

}