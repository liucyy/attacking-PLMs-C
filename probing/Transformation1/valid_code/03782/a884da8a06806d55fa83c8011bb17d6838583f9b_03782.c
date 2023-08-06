static void code_gen_alloc(unsigned long tb_size)

{

#ifdef USE_STATIC_CODE_GEN_BUFFER

    code_gen_buffer = static_code_gen_buffer;

    code_gen_buffer_size = DEFAULT_CODE_GEN_BUFFER_SIZE;

    map_exec(code_gen_buffer, code_gen_buffer_size);

#else

    code_gen_buffer_size = tb_size;

    if (code_gen_buffer_size == 0) {

#if defined(CONFIG_USER_ONLY)

        

        code_gen_buffer_size = DEFAULT_CODE_GEN_BUFFER_SIZE;

#else

        

        code_gen_buffer_size = (unsigned long)(ram_size / 4);

#endif

    }

    if (code_gen_buffer_size < MIN_CODE_GEN_BUFFER_SIZE)

        code_gen_buffer_size = MIN_CODE_GEN_BUFFER_SIZE;

    

#if defined(__linux__) 

    {

        int flags;

        void *start = NULL;



        flags = MAP_PRIVATE | MAP_ANONYMOUS;

#if defined(__x86_64__)

        flags |= MAP_32BIT;

        

        if (code_gen_buffer_size > (800 * 1024 * 1024))

            code_gen_buffer_size = (800 * 1024 * 1024);

#elif defined(__sparc_v9__)

        

        flags |= MAP_FIXED;

        start = (void *) 0x60000000UL;

        if (code_gen_buffer_size > (512 * 1024 * 1024))

            code_gen_buffer_size = (512 * 1024 * 1024);

#elif defined(__arm__)

        

        flags |= MAP_FIXED;

        start = (void *) 0x01000000UL;

        if (code_gen_buffer_size > 16 * 1024 * 1024)

            code_gen_buffer_size = 16 * 1024 * 1024;

#elif defined(__s390x__)

        

        

        if (code_gen_buffer_size > (3ul * 1024 * 1024 * 1024)) {

            code_gen_buffer_size = 3ul * 1024 * 1024 * 1024;

        }

        start = (void *)0x90000000UL;

#endif

        code_gen_buffer = mmap(start, code_gen_buffer_size,

                               PROT_WRITE | PROT_READ | PROT_EXEC,

                               flags, -1, 0);

        if (code_gen_buffer == MAP_FAILED) {

            fprintf(stderr, "Could not allocate dynamic translator buffer\n");

            exit(1);

        }

    }

#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__) \

    || defined(__DragonFly__) || defined(__OpenBSD__)

    {

        int flags;

        void *addr = NULL;

        flags = MAP_PRIVATE | MAP_ANONYMOUS;

#if defined(__x86_64__)

        

        flags |= MAP_FIXED;

        addr = (void *)0x40000000;

        

        if (code_gen_buffer_size > (800 * 1024 * 1024))

            code_gen_buffer_size = (800 * 1024 * 1024);

#elif defined(__sparc_v9__)

        

        flags |= MAP_FIXED;

        addr = (void *) 0x60000000UL;

        if (code_gen_buffer_size > (512 * 1024 * 1024)) {

            code_gen_buffer_size = (512 * 1024 * 1024);

        }

#endif

        code_gen_buffer = mmap(addr, code_gen_buffer_size,

                               PROT_WRITE | PROT_READ | PROT_EXEC, 

                               flags, -1, 0);

        if (code_gen_buffer == MAP_FAILED) {

            fprintf(stderr, "Could not allocate dynamic translator buffer\n");

            exit(1);

        }

    }

#else

    code_gen_buffer = qemu_malloc(code_gen_buffer_size);

    map_exec(code_gen_buffer, code_gen_buffer_size);

#endif

#endif 

    map_exec(code_gen_prologue, sizeof(code_gen_prologue));

    code_gen_buffer_max_size = code_gen_buffer_size - 

        (TCG_MAX_OP_SIZE * OPC_MAX_SIZE);

    code_gen_max_blocks = code_gen_buffer_size / CODE_GEN_AVG_BLOCK_SIZE;

    tbs = qemu_malloc(code_gen_max_blocks * sizeof(TranslationBlock));

}