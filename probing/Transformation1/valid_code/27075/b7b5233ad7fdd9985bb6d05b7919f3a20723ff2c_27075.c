static void *bsd_vmalloc(size_t size)

{

    void *p;

    mmap_lock();

    

    p = mmap(NULL, size, PROT_READ | PROT_WRITE,

             MAP_PRIVATE | MAP_ANON, -1, 0);



    if (h2g_valid(p)) {

        

        abi_ulong addr = h2g(p);

        page_set_flags(addr & TARGET_PAGE_MASK, TARGET_PAGE_ALIGN(addr + size),

                       PAGE_RESERVED);

    }



    mmap_unlock();

    return p;

}