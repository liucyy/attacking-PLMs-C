static abi_ulong setup_arg_pages(abi_ulong p, struct linux_binprm *bprm,

                                 struct image_info *info)

{

    abi_ulong stack_base, size, error;

    int i;



    

    size = x86_stack_size;

    if (size < MAX_ARG_PAGES*TARGET_PAGE_SIZE)

        size = MAX_ARG_PAGES*TARGET_PAGE_SIZE;

    error = target_mmap(0,

                        size + qemu_host_page_size,

                        PROT_READ | PROT_WRITE,

                        MAP_PRIVATE | MAP_ANONYMOUS,

                        -1, 0);

    if (error == -1) {

        perror("stk mmap");

        exit(-1);

    }

    

    target_mprotect(error + size, qemu_host_page_size, PROT_NONE);



    stack_base = error + size - MAX_ARG_PAGES*TARGET_PAGE_SIZE;

    p += stack_base;



    for (i = 0 ; i < MAX_ARG_PAGES ; i++) {

	if (bprm->page[i]) {

	    info->rss++;

            

	    memcpy_to_target(stack_base, bprm->page[i], TARGET_PAGE_SIZE);

	    free(bprm->page[i]);

	}

        stack_base += TARGET_PAGE_SIZE;

    }

    return p;

}