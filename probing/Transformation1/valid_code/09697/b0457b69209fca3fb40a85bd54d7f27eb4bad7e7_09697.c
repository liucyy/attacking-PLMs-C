static void n8x0_init(ram_addr_t ram_size, const char *boot_device,

                const char *kernel_filename,

                const char *kernel_cmdline, const char *initrd_filename,

                const char *cpu_model, struct arm_boot_info *binfo, int model)

{

    struct n800_s *s = (struct n800_s *) qemu_mallocz(sizeof(*s));

    int sdram_size = binfo->ram_size;

    int onenandram_size = 0x00010000;

    DisplayState *ds;



    if (ram_size < sdram_size + onenandram_size + OMAP242X_SRAM_SIZE) {

        fprintf(stderr, "This architecture uses %i bytes of memory\n",

                        sdram_size + onenandram_size + OMAP242X_SRAM_SIZE);

        exit(1);

    }



    s->cpu = omap2420_mpu_init(sdram_size, cpu_model);



    

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

    if (usb_enabled)

        n8x0_usb_setup(s);



    



    

    s->cpu->env->regs[15] = 0;



    if (kernel_filename) {

        

        binfo->kernel_filename = kernel_filename;

        binfo->kernel_cmdline = kernel_cmdline;

        binfo->initrd_filename = initrd_filename;

        arm_load_kernel(s->cpu->env, binfo);



        qemu_register_reset(n8x0_boot_init, s);

        n8x0_boot_init(s);

    }



    if (option_rom[0] && (boot_device[0] == 'n' || !kernel_filename)) {

        int rom_size;

        uint8_t nolo_tags[0x10000];

        

        s->cpu->env->regs[15] = OMAP2_Q2_BASE + 0x400000;



        

        rom_size = load_image_targphys(option_rom[0],

                                       OMAP2_Q2_BASE + 0x400000,

                                       sdram_size - 0x400000);

        printf("%i bytes of image loaded\n", rom_size);



        n800_setup_nolo_tags(nolo_tags);

        cpu_physical_memory_write(OMAP2_SRAM_BASE, nolo_tags, 0x10000);

    }

    

    ds = get_displaystate();

    ds->surface = qemu_resize_displaysurface(ds, 800, 480);

    dpy_resize(ds);

}