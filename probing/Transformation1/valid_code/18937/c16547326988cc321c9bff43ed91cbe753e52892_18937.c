static void n8x0_init(QEMUMachineInitArgs *args,

                      struct arm_boot_info *binfo, int model)

{

    MemoryRegion *sysmem = get_system_memory();

    struct n800_s *s = (struct n800_s *) g_malloc0(sizeof(*s));

    int sdram_size = binfo->ram_size;



    s->mpu = omap2420_mpu_init(sysmem, sdram_size, args->cpu_model);



    

    n8x0_gpio_setup(s);

    n8x0_nand_setup(s);

    n8x0_i2c_setup(s);

    if (model == 800)

        n800_tsc_kbd_setup(s);

    else if (model == 810) {

        n810_tsc_setup(s);

        n810_kbd_setup(s);

    }

    n8x0_spi_setup(s);

    n8x0_dss_setup(s);

    n8x0_cbus_setup(s);

    n8x0_uart_setup(s);

    if (usb_enabled(false)) {

        n8x0_usb_setup(s);

    }



    if (args->kernel_filename) {

        

        binfo->kernel_filename = args->kernel_filename;

        binfo->kernel_cmdline = args->kernel_cmdline;

        binfo->initrd_filename = args->initrd_filename;

        arm_load_kernel(s->mpu->cpu, binfo);



        qemu_register_reset(n8x0_boot_init, s);

    }



    if (option_rom[0].name &&

        (args->boot_device[0] == 'n' || !args->kernel_filename)) {

        uint8_t nolo_tags[0x10000];

        

        s->mpu->cpu->env.regs[15] = OMAP2_Q2_BASE + 0x400000;



        

        load_image_targphys(option_rom[0].name,

                            OMAP2_Q2_BASE + 0x400000,

                            sdram_size - 0x400000);



        n800_setup_nolo_tags(nolo_tags);

        cpu_physical_memory_write(OMAP2_SRAM_BASE, nolo_tags, 0x10000);

    }

}