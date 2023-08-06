static uint64_t ppc_radix64_walk_tree(PowerPCCPU *cpu, int rwx, vaddr eaddr,

                                      uint64_t base_addr, uint64_t nls,

                                      hwaddr *raddr, int *psize,

                                      int *fault_cause, int *prot,

                                      hwaddr *pte_addr)

{

    CPUState *cs = CPU(cpu);

    uint64_t index, pde;



    if (nls < 5) { 

        *fault_cause |= DSISR_R_BADCONFIG;

        return 0;

    }



    

    index = eaddr >> (*psize - nls); 

    index &= ((1UL << nls) - 1); 

    pde = ldq_phys(cs->as, base_addr + (index * sizeof(pde)));

    if (!(pde & R_PTE_VALID)) { 

        *fault_cause |= DSISR_NOPTE;

        return 0;

    }



    *psize -= nls;



    

    if (pde & R_PTE_LEAF) {

        uint64_t rpn = pde & R_PTE_RPN;

        uint64_t mask = (1UL << *psize) - 1;



        if (ppc_radix64_check_prot(cpu, rwx, pde, fault_cause, prot)) {

            return 0; 

        }



        

        *raddr = (rpn & ~mask) | (eaddr & mask);

        *pte_addr = base_addr + (index * sizeof(pde));

        return pde;

    }



    

    return ppc_radix64_walk_tree(cpu, rwx, eaddr, pde & R_PDE_NLB,

                                 pde & R_PDE_NLS, raddr, psize,

                                 fault_cause, prot, pte_addr);

}