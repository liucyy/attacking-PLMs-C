static int mmap_frag(abi_ulong real_start,

                     abi_ulong start, abi_ulong end,

                     int prot, int flags, int fd, abi_ulong offset)

{

    abi_ulong real_end, addr;

    void *host_start;

    int prot1, prot_new;



    real_end = real_start + qemu_host_page_size;

    host_start = g2h(real_start);



    

    prot1 = 0;

    for(addr = real_start; addr < real_end; addr++) {

        if (addr < start || addr >= end)

            prot1 |= page_get_flags(addr);

    }



    if (prot1 == 0) {

        

        void *p = mmap(host_start, qemu_host_page_size, prot,

                       flags | MAP_ANONYMOUS, -1, 0);

        if (p == MAP_FAILED)

            return -1;

        prot1 = prot;

    }

    prot1 &= PAGE_BITS;



    prot_new = prot | prot1;

    if (!(flags & MAP_ANONYMOUS)) {

        

        if ((flags & MAP_TYPE) == MAP_SHARED &&

            (prot & PROT_WRITE))

            return -EINVAL;



        

        if (!(prot1 & PROT_WRITE))

            mprotect(host_start, qemu_host_page_size, prot1 | PROT_WRITE);



        

        if (pread(fd, g2h(start), end - start, offset) == -1)

            return -1;



        

        if (prot_new != (prot1 | PROT_WRITE))

            mprotect(host_start, qemu_host_page_size, prot_new);

    } else {

        

        if (prot_new != prot1) {

            mprotect(host_start, qemu_host_page_size, prot_new);

        }

    }

    return 0;

}