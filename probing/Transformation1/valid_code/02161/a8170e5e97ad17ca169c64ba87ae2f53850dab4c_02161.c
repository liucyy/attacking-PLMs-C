static void set_kernel_args_old(const struct arm_boot_info *info)

{

    target_phys_addr_t p;

    const char *s;

    int initrd_size = info->initrd_size;

    target_phys_addr_t base = info->loader_start;



    

    p = base + KERNEL_ARGS_ADDR;

    

    WRITE_WORD(p, 4096);

    

    WRITE_WORD(p, info->ram_size / 4096);

    

    WRITE_WORD(p, 0);

#define FLAG_READONLY	1

#define FLAG_RDLOAD	4

#define FLAG_RDPROMPT	8

    

    WRITE_WORD(p, FLAG_READONLY | FLAG_RDLOAD | FLAG_RDPROMPT);

    

    WRITE_WORD(p, (31 << 8) | 0);	

    

    WRITE_WORD(p, 0);

    

    WRITE_WORD(p, 0);

    

    WRITE_WORD(p, 0);

    

    WRITE_WORD(p, 0);

    

    WRITE_WORD(p, 0);

    

    

    

    

    WRITE_WORD(p, 0);

    

    WRITE_WORD(p, 0);

    WRITE_WORD(p, 0);

    WRITE_WORD(p, 0);

    WRITE_WORD(p, 0);

    

    WRITE_WORD(p, 0);

    

    if (initrd_size)

        WRITE_WORD(p, info->loader_start + INITRD_LOAD_ADDR);

    else

        WRITE_WORD(p, 0);

    

    WRITE_WORD(p, initrd_size);

    

    WRITE_WORD(p, 0);

    

    WRITE_WORD(p, 0);

    

    WRITE_WORD(p, 0);

    

    WRITE_WORD(p, 0);

    

    WRITE_WORD(p, 0);

    

    while (p < base + KERNEL_ARGS_ADDR + 256 + 1024) {

        WRITE_WORD(p, 0);

    }

    s = info->kernel_cmdline;

    if (s) {

        cpu_physical_memory_write(p, (void *)s, strlen(s) + 1);

    } else {

        WRITE_WORD(p, 0);

    }

}