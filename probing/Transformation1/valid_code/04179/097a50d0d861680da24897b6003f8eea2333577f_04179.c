void *qemu_ram_mmap(int fd, size_t size, size_t align, bool shared)

{

    

    size_t total = size + align;

#if defined(__powerpc64__) && defined(__linux__)

    

    int anonfd = fd == -1 || qemu_fd_getpagesize(fd) == getpagesize() ? -1 : fd;

    int flags = anonfd == -1 ? MAP_ANONYMOUS : MAP_NORESERVE;

    void *ptr = mmap(0, total, PROT_NONE, flags | MAP_PRIVATE, anonfd, 0);

#else

    void *ptr = mmap(0, total, PROT_NONE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

#endif

    size_t offset = QEMU_ALIGN_UP((uintptr_t)ptr, align) - (uintptr_t)ptr;

    void *ptr1;



    if (ptr == MAP_FAILED) {

        return MAP_FAILED;

    }



    

    assert(!(align & (align - 1)));

    

    assert(align >= getpagesize());



    ptr1 = mmap(ptr + offset, size, PROT_READ | PROT_WRITE,

                MAP_FIXED |

                (fd == -1 ? MAP_ANONYMOUS : 0) |

                (shared ? MAP_SHARED : MAP_PRIVATE),

                fd, 0);

    if (ptr1 == MAP_FAILED) {

        munmap(ptr, total);

        return MAP_FAILED;

    }



    ptr += offset;

    total -= offset;



    if (offset > 0) {

        munmap(ptr - offset, offset);

    }



    

    if (total > size + getpagesize()) {

        munmap(ptr + size + getpagesize(), total - size - getpagesize());

    }



    return ptr;

}