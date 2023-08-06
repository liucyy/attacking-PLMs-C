void *qemu_vmalloc(size_t size)

{

    

    if (!size) {

        abort();

    }

    return oom_check(VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE));

}