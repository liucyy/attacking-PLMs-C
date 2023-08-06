int ppc_radix64_handle_mmu_fault(PowerPCCPU *cpu, vaddr eaddr, int rwx,

                                 int mmu_idx)

{

    CPUState *cs = CPU(cpu);

    CPUPPCState *env = &cpu->env;

    PPCVirtualHypervisorClass *vhc =

        PPC_VIRTUAL_HYPERVISOR_GET_CLASS(cpu->vhyp);

    hwaddr raddr, pte_addr;

    uint64_t lpid = 0, pid = 0, offset, size, patbe, prtbe0, pte;

    int page_size, prot, fault_cause = 0;



    assert((rwx == 0) || (rwx == 1) || (rwx == 2));

    assert(!msr_hv); 

    assert(cpu->vhyp);

    assert(ppc64_use_proc_tbl(cpu));



    

    if (((rwx == 2) && (msr_ir == 0)) || ((rwx != 2) && (msr_dr == 0))) {

        

        raddr = eaddr & 0x0FFFFFFFFFFFFFFFULL;



        tlb_set_page(cs, eaddr & TARGET_PAGE_MASK, raddr & TARGET_PAGE_MASK,

                     PAGE_READ | PAGE_WRITE | PAGE_EXEC, mmu_idx,

                     TARGET_PAGE_SIZE);

        return 0;

    }



    

    if (!ppc_radix64_get_fully_qualified_addr(env, eaddr, &lpid, &pid)) {

        ppc_radix64_raise_segi(cpu, rwx, eaddr);

        return 1;

    }



    

    patbe = vhc->get_patbe(cpu->vhyp);



    

    offset = pid * sizeof(struct prtb_entry);

    size = 1ULL << ((patbe & PATBE1_R_PRTS) + 12);

    if (offset >= size) {

        

        ppc_radix64_raise_si(cpu, rwx, eaddr, DSISR_NOPTE);

        return 1;

    }

    prtbe0 = ldq_phys(cs->as, (patbe & PATBE1_R_PRTB) + offset);



    

    page_size = PRTBE_R_GET_RTS(prtbe0);

    pte = ppc_radix64_walk_tree(cpu, rwx, eaddr & R_EADDR_MASK,

                                prtbe0 & PRTBE_R_RPDB, prtbe0 & PRTBE_R_RPDS,

                                &raddr, &page_size, &fault_cause, &prot,

                                &pte_addr);

    if (!pte) {

        ppc_radix64_raise_si(cpu, rwx, eaddr, fault_cause);

        return 1;

    }



    

    ppc_radix64_set_rc(cpu, rwx, pte, pte_addr, &prot);



    tlb_set_page(cs, eaddr & TARGET_PAGE_MASK, raddr & TARGET_PAGE_MASK,

                 prot, mmu_idx, 1UL << page_size);

    return 0;

}