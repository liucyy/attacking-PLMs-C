int get_physical_address(CPUState * env, target_ulong * physical,

			 int *prot, target_ulong address,

			 int rw, int access_type)

{

    

    if ((address >= 0x80000000 && address < 0xc0000000) ||

	address >= 0xe0000000) {

	if (!(env->sr & SR_MD)

	    && (address < 0xe0000000 || address > 0xe4000000)) {

	    

	    fprintf(stderr, "Unauthorized access\n");

	    return (rw & PAGE_WRITE) ? MMU_DTLB_MISS_WRITE :

		MMU_DTLB_MISS_READ;

	}

	

	*physical = address & 0x1FFFFFFF;

	*prot = PAGE_READ | PAGE_WRITE;

	return MMU_OK;

    }



    

    if (!env->mmucr & MMUCR_AT) {

	*physical = address & 0x1FFFFFFF;

	*prot = PAGE_READ | PAGE_WRITE;

	return MMU_OK;

    }



    

    return get_mmu_address(env, physical, prot, address, rw, access_type);

}