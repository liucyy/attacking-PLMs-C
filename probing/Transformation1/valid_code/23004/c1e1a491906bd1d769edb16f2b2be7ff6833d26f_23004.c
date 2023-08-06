void bareetraxfs_init (ram_addr_t ram_size, int vga_ram_size,

                       const char *boot_device, DisplayState *ds,

                       const char *kernel_filename, const char *kernel_cmdline,

                       const char *initrd_filename, const char *cpu_model)

{

    CPUState *env;

    struct etraxfs_pic *pic;

    void *etraxfs_dmac;

    struct etraxfs_dma_client *eth[2] = {NULL, NULL};

    int kernel_size;

    int i;

    ram_addr_t phys_ram;

    ram_addr_t phys_flash;

    ram_addr_t phys_intmem;



    

    if (cpu_model == NULL) {

        cpu_model = "crisv32";

    }

    env = cpu_init(cpu_model);

    qemu_register_reset(main_cpu_reset, env);



    

    phys_ram = qemu_ram_alloc(ram_size);

    cpu_register_physical_memory(0x40000000, ram_size, phys_ram | IO_MEM_RAM);



    

    phys_intmem = qemu_ram_alloc(INTMEM_SIZE);

    cpu_register_physical_memory(0x38000000, INTMEM_SIZE,

                                 phys_intmem | IO_MEM_RAM);





    phys_flash = qemu_ram_alloc(FLASH_SIZE);

    i = drive_get_index(IF_PFLASH, 0, 0);

    pflash_cfi02_register(0x0, phys_flash,

                          i != -1 ? drives_table[i].bdrv : NULL, (64 * 1024),

                          FLASH_SIZE >> 16,

                          1, 2, 0x0000, 0x0000, 0x0000, 0x0000,

                          0x555, 0x2aa);

    pic = etraxfs_pic_init(env, 0x3001c000);

    etraxfs_dmac = etraxfs_dmac_init(env, 0x30000000, 10);

    for (i = 0; i < 10; i++) {

        

        etraxfs_dmac_connect(etraxfs_dmac, i, pic->irq + 7 + i, i & 1);

    }



    

    eth[0] = etraxfs_eth_init(&nd_table[0], env, pic->irq + 25, 0x30034000);

    if (nb_nics > 1)

        eth[1] = etraxfs_eth_init(&nd_table[1], env, pic->irq + 26, 0x30036000);



    

    etraxfs_dmac_connect_client(etraxfs_dmac, 0, eth[0]);

    etraxfs_dmac_connect_client(etraxfs_dmac, 1, eth[0] + 1);

    if (eth[1]) {

        etraxfs_dmac_connect_client(etraxfs_dmac, 6, eth[1]);

        etraxfs_dmac_connect_client(etraxfs_dmac, 7, eth[1] + 1);

    }



    

    etraxfs_timer_init(env, pic->irq + 0x1b, pic->nmi + 1, 0x3001e000);

    etraxfs_timer_init(env, pic->irq + 0x1b, pic->nmi + 1, 0x3005e000);



    for (i = 0; i < 4; i++) {

        if (serial_hds[i]) {

            etraxfs_ser_init(env, pic->irq + 0x14 + i,

                             serial_hds[i], 0x30026000 + i * 0x2000);

        }

    }



    if (kernel_filename) {

        uint64_t entry, high;

        int kcmdline_len;



        

        kernel_size = load_elf(kernel_filename, -0x80000000LL,

                               &entry, NULL, &high);

        bootstrap_pc = entry;

        if (kernel_size < 0) {

            

            kernel_size = load_image(kernel_filename, phys_ram_base + 0x4000);

            bootstrap_pc = 0x40004000;

            env->regs[9] = 0x40004000 + kernel_size;

        }

        env->regs[8] = 0x56902387; 



        if (kernel_cmdline && (kcmdline_len = strlen(kernel_cmdline))) {

            if (kcmdline_len > 256) {

                fprintf(stderr, "Too long CRIS kernel cmdline (max 256)\n");

                exit(1);

            }

            pstrcpy_targphys(high, 256, kernel_cmdline);

            

            env->regs[10] = 0x87109563;

            env->regs[11] = high;

        }

    }

    env->pc = bootstrap_pc;



    printf ("pc =%x\n", env->pc);

    printf ("ram size =%ld\n", ram_size);

}