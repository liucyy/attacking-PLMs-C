static unsigned hpte_page_shift(const struct ppc_one_seg_page_size *sps,

    uint64_t pte0, uint64_t pte1)

{

    int i;



    if (!(pte0 & HPTE64_V_LARGE)) {

        if (sps->page_shift != 12) {

            

            return 0;

        }

        

        return 12;

    }



    for (i = 0; i < PPC_PAGE_SIZES_MAX_SZ; i++) {

        const struct ppc_one_page_size *ps = &sps->enc[i];

        uint64_t mask;



        if (!ps->page_shift) {

            break;

        }



        if (ps->page_shift == 12) {

            

            continue;

        }



        mask = ((1ULL << ps->page_shift) - 1) & HPTE64_R_RPN;



        if ((pte1 & mask) == (ps->pte_enc << HPTE64_R_RPN_SHIFT)) {

            return ps->page_shift;

        }

    }



    return 0; 

}