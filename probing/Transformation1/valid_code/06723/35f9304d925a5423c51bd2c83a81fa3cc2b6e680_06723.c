static inline int lock_hpte(void *hpte, target_ulong bits)

{

    uint64_t pteh;



    pteh = ldq_p(hpte);



    

    if (pteh & bits) {

        return 0;

    }

    stq_p(hpte, pteh | HPTE_V_HVLOCK);

    return 1;

}