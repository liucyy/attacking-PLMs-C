void *qemu_anon_ram_alloc(size_t size)

{

    void *ptr;



    

    ptr = VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE);

    trace_qemu_anon_ram_alloc(size, ptr);

    return ptr;

}