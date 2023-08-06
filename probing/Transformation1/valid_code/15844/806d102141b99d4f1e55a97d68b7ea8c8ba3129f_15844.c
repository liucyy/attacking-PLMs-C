unsigned long init_guest_space(unsigned long host_start,

                               unsigned long host_size,

                               unsigned long guest_start,

                               bool fixed)

{

    unsigned long current_start, real_start;

    int flags;



    assert(host_start || host_size);



    

    if (host_start && !host_size) {

        if (guest_validate_base(host_start)) {

            return host_start;

        } else {

            return (unsigned long)-1;

        }

    }



    

    current_start = host_start & qemu_host_page_mask;

    flags = MAP_ANONYMOUS | MAP_PRIVATE | MAP_NORESERVE;

    if (fixed) {

        flags |= MAP_FIXED;

    }



    

    while (1) {

        

        real_start = (unsigned long)

            mmap((void *)current_start, host_size, PROT_NONE, flags, -1, 0);

        if (real_start == (unsigned long)-1) {

            return (unsigned long)-1;

        }



        if ((real_start == current_start)

            && guest_validate_base(real_start - guest_start)) {

            break;

        }



        

        munmap((void *)real_start, host_size);

        current_start += qemu_host_page_size;

        if (host_start == current_start) {

            

            return (unsigned long)-1;

        }

    }



    return real_start;

}