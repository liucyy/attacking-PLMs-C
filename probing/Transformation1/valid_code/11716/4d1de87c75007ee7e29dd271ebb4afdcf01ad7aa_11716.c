abi_long do_brk(abi_ulong new_brk)

{

    abi_ulong brk_page;

    abi_long mapped_addr;

    int	new_alloc_size;



    if (!new_brk)

        return target_brk;

    if (new_brk < target_original_brk)

        return target_brk;



    brk_page = HOST_PAGE_ALIGN(target_brk);



    

    if (new_brk < brk_page) {

	target_brk = new_brk;

    	return target_brk;

    }



    

    new_alloc_size = HOST_PAGE_ALIGN(new_brk - brk_page + 1);

    mapped_addr = get_errno(target_mmap(brk_page, new_alloc_size,

                                        PROT_READ|PROT_WRITE,

                                        MAP_ANON|MAP_PRIVATE, 0, 0));



    if (mapped_addr == brk_page) {

        target_brk = new_brk;

        return target_brk;

    } else if (mapped_addr != -1) {

        

        target_munmap(mapped_addr, new_alloc_size);

        mapped_addr = -1;

    }



#if defined(TARGET_ALPHA)

    

    return -TARGET_ENOMEM;

#endif

    

    return target_brk;

}