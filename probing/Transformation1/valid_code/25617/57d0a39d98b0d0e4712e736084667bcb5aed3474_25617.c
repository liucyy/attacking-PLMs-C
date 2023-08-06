static int ppc_hash64_pte_prot(CPUPPCState *env,

                               ppc_slb_t *slb, ppc_hash_pte64_t pte)

{

    unsigned pp, key;

    

    int prot = 0;



    key = !!(msr_pr ? (slb->vsid & SLB_VSID_KP)

             : (slb->vsid & SLB_VSID_KS));

    pp = (pte.pte1 & HPTE64_R_PP) | ((pte.pte1 & HPTE64_R_PP0) >> 61);



    if (key == 0) {

        switch (pp) {

        case 0x0:

        case 0x1:

        case 0x2:

            prot = PAGE_READ | PAGE_WRITE;

            break;



        case 0x3:

        case 0x6:

            prot = PAGE_READ;

            break;

        }

    } else {

        switch (pp) {

        case 0x0:

        case 0x6:

            prot = 0;

            break;



        case 0x1:

        case 0x3:

            prot = PAGE_READ;

            break;



        case 0x2:

            prot = PAGE_READ | PAGE_WRITE;

            break;

        }

    }



    

    if (!(pte.pte1 & HPTE64_R_N) || (pte.pte1 & HPTE64_R_G)) {

        prot |= PAGE_EXEC;

    }



    return prot;

}