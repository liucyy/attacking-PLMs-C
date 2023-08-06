static void *file_ram_alloc(RAMBlock *block,

                            ram_addr_t memory,

                            const char *path,

                            Error **errp)

{

    bool unlink_on_error = false;

    char *filename;

    char *sanitized_name;

    char *c;

    void *area = MAP_FAILED;

    int fd = -1;

    int64_t file_size;



    if (kvm_enabled() && !kvm_has_sync_mmu()) {

        error_setg(errp,

                   "host lacks kvm mmu notifiers, -mem-path unsupported");

        return NULL;

    }



    for (;;) {

        fd = open(path, O_RDWR);

        if (fd >= 0) {

            

            break;

        }

        if (errno == ENOENT) {

            

            fd = open(path, O_RDWR | O_CREAT | O_EXCL, 0644);

            if (fd >= 0) {

                unlink_on_error = true;

                break;

            }

        } else if (errno == EISDIR) {

            

            

            sanitized_name = g_strdup(memory_region_name(block->mr));

            for (c = sanitized_name; *c != '\0'; c++) {

                if (*c == '/') {

                    *c = '_';

                }

            }



            filename = g_strdup_printf("%s/qemu_back_mem.%s.XXXXXX", path,

                                       sanitized_name);

            g_free(sanitized_name);



            fd = mkstemp(filename);

            if (fd >= 0) {

                unlink(filename);

                g_free(filename);

                break;

            }

            g_free(filename);

        }

        if (errno != EEXIST && errno != EINTR) {

            error_setg_errno(errp, errno,

                             "can't open backing store %s for guest RAM",

                             path);

            goto error;

        }

        

    }



    block->page_size = qemu_fd_getpagesize(fd);

    block->mr->align = block->page_size;

#if defined(__s390x__)

    if (kvm_enabled()) {

        block->mr->align = MAX(block->mr->align, QEMU_VMALLOC_ALIGN);

    }

#endif



    file_size = get_file_size(fd);



    if (memory < block->page_size) {

        error_setg(errp, "memory size 0x" RAM_ADDR_FMT " must be equal to "

                   "or larger than page size 0x%zx",

                   memory, block->page_size);

        goto error;

    }



    if (file_size > 0 && file_size < memory) {

        error_setg(errp, "backing store %s size 0x%" PRIx64

                   " does not match 'size' option 0x" RAM_ADDR_FMT,

                   path, file_size, memory);

        goto error;

    }



    memory = ROUND_UP(memory, block->page_size);



    

    if (!file_size && ftruncate(fd, memory)) {

        perror("ftruncate");

    }



    area = qemu_ram_mmap(fd, memory, block->mr->align,

                         block->flags & RAM_SHARED);

    if (area == MAP_FAILED) {

        error_setg_errno(errp, errno,

                         "unable to map backing store for guest RAM");

        goto error;

    }



    if (mem_prealloc) {

        os_mem_prealloc(fd, area, memory, errp);

        if (errp && *errp) {

            goto error;

        }

    }



    block->fd = fd;

    return area;



error:

    if (area != MAP_FAILED) {

        qemu_ram_munmap(area, memory);

    }

    if (unlink_on_error) {

        unlink(path);

    }

    if (fd != -1) {

        close(fd);

    }

    return NULL;

}