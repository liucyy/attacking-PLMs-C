static int validate_guest_space(unsigned long guest_base,

                                unsigned long guest_size)

{

    unsigned long real_start, test_page_addr;



    

    test_page_addr = guest_base + (0xffff0f00 & qemu_host_page_mask);



    

    if (test_page_addr >= guest_base

        && test_page_addr <= (guest_base + guest_size)) {

        return -1;

    }



    

    real_start = (unsigned long)

                 mmap((void *)test_page_addr, qemu_host_page_size,

                     PROT_READ | PROT_WRITE,

                     MAP_ANONYMOUS | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);



    

    if (real_start == -1ul) {

        return 0;

    }



    if (real_start != test_page_addr) {

        

        munmap((void *)real_start, qemu_host_page_size);

        return 0;

    }



    



    

    __put_user(5, (uint32_t *)g2h(0xffff0ffcul));



    

    if (mprotect((void *)test_page_addr, qemu_host_page_size, PROT_READ)) {

        perror("Protecting guest commpage");

        exit(-1);

    }



    return 1; 

}