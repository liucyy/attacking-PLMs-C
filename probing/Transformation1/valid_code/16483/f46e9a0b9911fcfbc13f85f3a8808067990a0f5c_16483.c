static void zero_bss(abi_ulong elf_bss, abi_ulong last_bss, int prot)

{

    uintptr_t host_start, host_map_start, host_end;



    last_bss = TARGET_PAGE_ALIGN(last_bss);



    



    host_start = (uintptr_t) g2h(elf_bss);

    host_end = (uintptr_t) g2h(last_bss);

    host_map_start = (host_start + qemu_real_host_page_size - 1);

    host_map_start &= -qemu_real_host_page_size;



    if (host_map_start < host_end) {

        void *p = mmap((void *)host_map_start, host_end - host_map_start,

                       prot, MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

        if (p == MAP_FAILED) {

            perror("cannot mmap brk");

            exit(-1);

        }



        

        page_set_flags(elf_bss & TARGET_PAGE_MASK, last_bss, prot|PAGE_VALID);

    }



    if (host_start < host_map_start) {

        memset((void *)host_start, 0, host_map_start - host_start);

    }

}