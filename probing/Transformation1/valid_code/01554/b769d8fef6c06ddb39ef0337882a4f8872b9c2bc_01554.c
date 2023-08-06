int cpu_sparc_handle_mmu_fault (CPUState *env, uint32_t address, int rw,

                              int is_user, int is_softmmu)

{

    int exception = 0;

    int access_type, access_perms = 0, access_index = 0;

    uint8_t *pde_ptr;

    uint32_t pde, virt_addr;

    int error_code = 0, is_dirty, prot, ret = 0;

    unsigned long paddr, vaddr, page_offset;



    access_type = env->access_type;

    if (env->user_mode_only) {

        

        ret = -2;

        goto do_fault;

    }



    virt_addr = address & TARGET_PAGE_MASK;

    if ((env->mmuregs[0] & MMU_E) == 0) { 

	paddr = address;

	page_offset = address & (TARGET_PAGE_SIZE - 1);

        prot = PAGE_READ | PAGE_WRITE;

        goto do_mapping;

    }



    

    

    pde_ptr = phys_ram_base + (env->mmuregs[1] << 4) + (env->mmuregs[2] << 4);

    env->access_type = ACCESS_MMU;

    pde = ldl_raw(pde_ptr);



    

    switch (pde & PTE_ENTRYTYPE_MASK) {

    case 0: 

        error_code = 1;

        goto do_fault;

    case 2: 

    case 3: 

        error_code = 4;

        goto do_fault;

    case 1: 

	pde_ptr = phys_ram_base + ((address >> 22) & ~3) + ((pde & ~3) << 4);

	pde = ldl_raw(pde_ptr);



	switch (pde & PTE_ENTRYTYPE_MASK) {

	case 0: 

	    error_code = 1;

	    goto do_fault;

	case 3: 

	    error_code = 4;

	    goto do_fault;

	case 1: 

	    pde_ptr = phys_ram_base + ((address & 0xfc0000) >> 16) + ((pde & ~3) << 4);

	    pde = ldl_raw(pde_ptr);



	    switch (pde & PTE_ENTRYTYPE_MASK) {

	    case 0: 

		error_code = 1;

		goto do_fault;

	    case 3: 

		error_code = 4;

		goto do_fault;

	    case 1: 

		pde_ptr = phys_ram_base + ((address & 0x3f000) >> 10) + ((pde & ~3) << 4);

		pde = ldl_raw(pde_ptr);



		switch (pde & PTE_ENTRYTYPE_MASK) {

		case 0: 

		    error_code = 1;

		    goto do_fault;

		case 1: 

		case 3: 

		    error_code = 4;

		    goto do_fault;

		case 2: 

		    virt_addr = address & TARGET_PAGE_MASK;

		    page_offset = (address & TARGET_PAGE_MASK) & (TARGET_PAGE_SIZE - 1);

		}

		break;

	    case 2: 

		virt_addr = address & ~0x3ffff;

		page_offset = address & 0x3ffff;

	    }

	    break;

	case 2: 

	    virt_addr = address & ~0xffffff;

	    page_offset = address & 0xffffff;

	}

    }



    

    is_dirty = rw && !(pde & PG_MODIFIED_MASK);

    if (!(pde & PG_ACCESSED_MASK) || is_dirty) {

	pde |= PG_ACCESSED_MASK;

	if (is_dirty)

	    pde |= PG_MODIFIED_MASK;

	stl_raw(pde_ptr, pde);

    }



    

    access_index = (rw << 2) | ((access_type == ACCESS_CODE)? 2 : 0) | (is_user? 0 : 1);

    access_perms = (pde & PTE_ACCESS_MASK) >> PTE_ACCESS_SHIFT;

    error_code = access_table[access_index][access_perms];

    if (error_code)

	goto do_fault;



    

    prot = PAGE_READ;

    if (pde & PG_MODIFIED_MASK) {

        

	if (rw_table[is_user][access_perms])

	        prot |= PAGE_WRITE;

    }



    

    virt_addr = address & TARGET_PAGE_MASK;

    paddr = ((pde & PTE_ADDR_MASK) << 4) + page_offset;



 do_mapping:

    env->access_type = access_type;

    vaddr = virt_addr + ((address & TARGET_PAGE_MASK) & (TARGET_PAGE_SIZE - 1));



    ret = tlb_set_page(env, vaddr, paddr, prot, is_user, is_softmmu);

    return ret;



 do_fault:

    env->access_type = access_type;

    if (env->mmuregs[3]) 

	env->mmuregs[3] = 1; 

    env->mmuregs[3] |= (access_index << 5) | (error_code << 2) | 2;

    env->mmuregs[4] = address; 



    if (env->mmuregs[0] & MMU_NF) 

	return 0;



    env->exception_index = exception;

    env->error_code = error_code;

    return error_code;

}