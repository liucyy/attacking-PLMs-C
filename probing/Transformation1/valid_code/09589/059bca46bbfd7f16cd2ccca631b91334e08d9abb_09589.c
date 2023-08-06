int mmap_frag(unsigned long host_start,

               unsigned long start, unsigned long end,

               int prot, int flags, int fd, unsigned long offset)

{

    unsigned long host_end, ret, addr;

    int prot1, prot_new;



    host_end = host_start + qemu_host_page_size;



    

    prot1 = 0;

    for(addr = host_start; addr < host_end; addr++) {

        if (addr < start || addr >= end)

            prot1 |= page_get_flags(addr);

    }



    if (prot1 == 0) {

        

        ret = (long)mmap((void *)host_start, qemu_host_page_size, prot,

                         flags | MAP_ANONYMOUS, -1, 0);

        if (ret == -1)

            return ret;

    }

    prot1 &= PAGE_BITS;



    prot_new = prot | prot1;

    if (!(flags & MAP_ANONYMOUS)) {

        

#ifndef __APPLE__

        if ((flags & MAP_TYPE) == MAP_SHARED &&

#else

        if ((flags &  MAP_SHARED) &&

#endif

            (prot & PROT_WRITE))

            return -EINVAL;



        

        if (!(prot1 & PROT_WRITE))

            mprotect((void *)host_start, qemu_host_page_size, prot1 | PROT_WRITE);



        

        pread(fd, (void *)start, end - start, offset);



        

        if (prot_new != (prot1 | PROT_WRITE))

            mprotect((void *)host_start, qemu_host_page_size, prot_new);

    } else {

        

        if (prot_new != prot1) {

            mprotect((void *)host_start, qemu_host_page_size, prot_new);

        }

    }

    return 0;

}