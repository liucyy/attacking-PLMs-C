int ppc64_v3_handle_mmu_fault(PowerPCCPU *cpu, vaddr eaddr, int rwx,

                              int mmu_idx)

{

    if (ppc64_radix_guest(cpu)) { 

        

        error_report("Guest Radix Support Unimplemented");

        exit(1);

    } else { 

        return ppc_hash64_handle_mmu_fault(cpu, eaddr, rwx, mmu_idx);

    }

}