abi_ulong mmap_find_vma(abi_ulong start, abi_ulong size)

{

    void *ptr, *prev;

    abi_ulong addr;

    int wrapped, repeat;



    

    if (start == 0) {

        start = mmap_next_start;

    } else {

        start &= qemu_host_page_mask;

    }



    size = HOST_PAGE_ALIGN(size);



    if (RESERVED_VA) {

        return mmap_find_vma_reserved(start, size);

    }



    addr = start;

    wrapped = repeat = 0;

    prev = 0;



    for (;; prev = ptr) {

        

        ptr = mmap(g2h(addr), size, PROT_NONE,

                   MAP_ANONYMOUS|MAP_PRIVATE|MAP_NORESERVE, -1, 0);



        

        if (ptr == MAP_FAILED) {

            return (abi_ulong)-1;

        }



        

        repeat = (ptr == prev ? repeat + 1 : 0);



        if (h2g_valid(ptr + size - 1)) {

            addr = h2g(ptr);



            if ((addr & ~TARGET_PAGE_MASK) == 0) {

                

                if (start == mmap_next_start && addr >= TASK_UNMAPPED_BASE) {

                    mmap_next_start = addr + size;

                }

                return addr;

            }



            

            switch (repeat) {

            case 0:

                

                addr = TARGET_PAGE_ALIGN(addr);

                break;

            case 1:

                

                addr &= TARGET_PAGE_MASK;

                break;

            case 2:

                

                addr = 0;

                break;

            default:

                

                addr = -1;

                break;

            }

        } else {

            

            addr = (repeat ? -1 : 0);

        }



        

        munmap(ptr, size);



        

        if (addr == -1ul) {

            return (abi_ulong)-1;

        } else if (addr == 0) {

            if (wrapped) {

                return (abi_ulong)-1;

            }

            wrapped = 1;

            

            addr = (mmap_min_addr > TARGET_PAGE_SIZE

                     ? TARGET_PAGE_ALIGN(mmap_min_addr)

                     : TARGET_PAGE_SIZE);

        } else if (wrapped && addr >= start) {

            return (abi_ulong)-1;

        }

    }

}