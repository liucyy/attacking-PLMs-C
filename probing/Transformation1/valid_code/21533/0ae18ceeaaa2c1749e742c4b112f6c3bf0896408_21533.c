mips_mipssim_init (ram_addr_t ram_size, int vga_ram_size,

                   const char *boot_device, DisplayState *ds,

                   const char *kernel_filename, const char *kernel_cmdline,

                   const char *initrd_filename, const char *cpu_model)

{

    char buf[1024];

    unsigned long bios_offset;

    CPUState *env;

    int bios_size;



    

    if (cpu_model == NULL) {

#ifdef TARGET_MIPS64

        cpu_model = "5Kf";

#else

        cpu_model = "24Kf";

#endif

    }

    env = cpu_init(cpu_model);

    if (!env) {

        fprintf(stderr, "Unable to find CPU definition\n");

        exit(1);

    }

    qemu_register_reset(main_cpu_reset, env);



    

    cpu_register_physical_memory(0, ram_size, IO_MEM_RAM);



    

    bios_offset = ram_size + vga_ram_size;

    if (bios_name == NULL)

        bios_name = BIOS_FILENAME;

    snprintf(buf, sizeof(buf), "%s/%s", bios_dir, bios_name);

    bios_size = load_image(buf, phys_ram_base + bios_offset);

    if ((bios_size < 0 || bios_size > BIOS_SIZE) && !kernel_filename) {

        

        fprintf(stderr,

                "qemu: Could not load MIPS bios '%s', and no -kernel argument was specified\n",

                buf);

        exit(1);

    } else {

        

        cpu_register_physical_memory(0x1fc00000LL,

                                     bios_size, bios_offset | IO_MEM_ROM);

        

        env->active_tc.PC = (target_long)(int32_t)0xbfc00000;

    }



    if (kernel_filename) {

        loaderparams.ram_size = ram_size;

        loaderparams.kernel_filename = kernel_filename;

        loaderparams.kernel_cmdline = kernel_cmdline;

        loaderparams.initrd_filename = initrd_filename;

        load_kernel(env);

    }



    

    cpu_mips_irq_init_cpu(env);

    cpu_mips_clock_init(env);



    

    isa_mmio_init(0x1fd00000, 0x00010000);



    

    if (serial_hds[0])

        serial_init(0x3f8, env->irq[4], 115200, serial_hds[0]);



    if (nd_table[0].vlan) {

        if (nd_table[0].model == NULL

            || strcmp(nd_table[0].model, "mipsnet") == 0) {

            

            mipsnet_init(0x4200, env->irq[2], &nd_table[0]);

        } else if (strcmp(nd_table[0].model, "?") == 0) {

            fprintf(stderr, "qemu: Supported NICs: mipsnet\n");

            exit (1);

        } else {

            fprintf(stderr, "qemu: Unsupported NIC: %s\n", nd_table[0].model);

            exit (1);

        }

    }

}