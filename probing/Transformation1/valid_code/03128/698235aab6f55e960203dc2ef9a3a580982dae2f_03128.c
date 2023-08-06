static int get_physical_address(CPUState *env, target_phys_addr_t *physical,

                                int *prot, int *access_index,

                                target_ulong address, int rw, int mmu_idx)

{

    int access_perms = 0;

    target_phys_addr_t pde_ptr;

    uint32_t pde;

    int error_code = 0, is_dirty, is_user;

    unsigned long page_offset;



    is_user = mmu_idx == MMU_USER_IDX;



    if ((env->mmuregs[0] & MMU_E) == 0) { 

        

        if (rw == 2 && (env->mmuregs[0] & env->def->mmu_bm)) {

            *physical = env->prom_addr | (address & 0x7ffffULL);

            *prot = PAGE_READ | PAGE_EXEC;

            return 0;

        }

        *physical = address;

        *prot = PAGE_READ | PAGE_WRITE | PAGE_EXEC;

        return 0;

    }



    *access_index = ((rw & 1) << 2) | (rw & 2) | (is_user? 0 : 1);

    *physical = 0xffffffffffff0000ULL;



    

    

    pde_ptr = (env->mmuregs[1] << 4) + (env->mmuregs[2] << 2);

    pde = ldl_phys(pde_ptr);



    

    switch (pde & PTE_ENTRYTYPE_MASK) {

    default:

    case 0: 

        return 1 << 2;

    case 2: 

    case 3: 

        return 4 << 2;

    case 1: 

        pde_ptr = ((address >> 22) & ~3) + ((pde & ~3) << 4);

        pde = ldl_phys(pde_ptr);



        switch (pde & PTE_ENTRYTYPE_MASK) {

        default:

        case 0: 

            return (1 << 8) | (1 << 2);

        case 3: 

            return (1 << 8) | (4 << 2);

        case 1: 

            pde_ptr = ((address & 0xfc0000) >> 16) + ((pde & ~3) << 4);

            pde = ldl_phys(pde_ptr);



            switch (pde & PTE_ENTRYTYPE_MASK) {

            default:

            case 0: 

                return (2 << 8) | (1 << 2);

            case 3: 

                return (2 << 8) | (4 << 2);

            case 1: 

                pde_ptr = ((address & 0x3f000) >> 10) + ((pde & ~3) << 4);

                pde = ldl_phys(pde_ptr);



                switch (pde & PTE_ENTRYTYPE_MASK) {

                default:

                case 0: 

                    return (3 << 8) | (1 << 2);

                case 1: 

                case 3: 

                    return (3 << 8) | (4 << 2);

                case 2: 

                    page_offset = (address & TARGET_PAGE_MASK) &

                        (TARGET_PAGE_SIZE - 1);

                }

                break;

            case 2: 

                page_offset = address & 0x3ffff;

            }

            break;

        case 2: 

            page_offset = address & 0xffffff;

        }

    }



    

    is_dirty = (rw & 1) && !(pde & PG_MODIFIED_MASK);

    if (!(pde & PG_ACCESSED_MASK) || is_dirty) {

        pde |= PG_ACCESSED_MASK;

        if (is_dirty)

            pde |= PG_MODIFIED_MASK;

        stl_phys_notdirty(pde_ptr, pde);

    }

    

    access_perms = (pde & PTE_ACCESS_MASK) >> PTE_ACCESS_SHIFT;

    error_code = access_table[*access_index][access_perms];

    if (error_code && !((env->mmuregs[0] & MMU_NF) && is_user))

        return error_code;



    

    *prot = perm_table[is_user][access_perms];

    if (!(pde & PG_MODIFIED_MASK)) {

        

        *prot &= ~PAGE_WRITE;

    }



    

    *physical = ((target_phys_addr_t)(pde & PTE_ADDR_MASK) << 4) + page_offset;

    return error_code;

}