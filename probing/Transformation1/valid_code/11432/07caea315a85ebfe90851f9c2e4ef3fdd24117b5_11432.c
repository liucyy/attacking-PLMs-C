static void realview_init(ram_addr_t ram_size,

                     const char *boot_device,

                     const char *kernel_filename, const char *kernel_cmdline,

                     const char *initrd_filename, const char *cpu_model)

{

    CPUState *env;

    ram_addr_t ram_offset;

    DeviceState *dev;

    qemu_irq *irqp;

    qemu_irq pic[64];

    PCIBus *pci_bus;

    NICInfo *nd;

    int n;

    int done_smc = 0;

    qemu_irq cpu_irq[4];

    int ncpu;



    if (!cpu_model)

        cpu_model = "arm926";

    

    if (strcmp(cpu_model, "arm11mpcore") == 0) {

        ncpu = 4;

    } else {

        ncpu = 1;

    }



    for (n = 0; n < ncpu; n++) {

        env = cpu_init(cpu_model);

        if (!env) {

            fprintf(stderr, "Unable to find CPU definition\n");

            exit(1);

        }

        irqp = arm_pic_init_cpu(env);

        cpu_irq[n] = irqp[ARM_PIC_CPU_IRQ];

        if (n > 0) {

            

            env->regs[15] = 0x80000000;

        }

    }



    ram_offset = qemu_ram_alloc(ram_size);

    

    

    cpu_register_physical_memory(0, ram_size, ram_offset | IO_MEM_RAM);



    arm_sysctl_init(0x10000000, 0xc1400400);



    if (ncpu == 1) {

        

        dev = sysbus_create_simple("realview_gic", 0x10040000, cpu_irq[0]);

    } else {

        dev = sysbus_create_varargs("realview_mpcore", -1,

                                    cpu_irq[0], cpu_irq[1], cpu_irq[2],

                                    cpu_irq[3], NULL);

    }

    for (n = 0; n < 64; n++) {

        pic[n] = qdev_get_gpio_in(dev, n);

    }



    sysbus_create_simple("pl050_keyboard", 0x10006000, pic[20]);

    sysbus_create_simple("pl050_mouse", 0x10007000, pic[21]);



    sysbus_create_simple("pl011", 0x10009000, pic[12]);

    sysbus_create_simple("pl011", 0x1000a000, pic[13]);

    sysbus_create_simple("pl011", 0x1000b000, pic[14]);

    sysbus_create_simple("pl011", 0x1000c000, pic[15]);



    

    sysbus_create_simple("pl081", 0x10030000, pic[24]);



    sysbus_create_simple("sp804", 0x10011000, pic[4]);

    sysbus_create_simple("sp804", 0x10012000, pic[5]);



    sysbus_create_simple("pl110_versatile", 0x10020000, pic[23]);



    sysbus_create_varargs("pl181", 0x10005000, pic[17], pic[18], NULL);



    sysbus_create_simple("pl031", 0x10017000, pic[10]);



    dev = sysbus_create_varargs("realview_pci", 0x60000000,

                                pic[48], pic[49], pic[50], pic[51], NULL);

    pci_bus = (PCIBus *)qdev_get_child_bus(dev, "pci");

    if (usb_enabled) {

        usb_ohci_init_pci(pci_bus, -1);

    }

    n = drive_get_max_bus(IF_SCSI);

    while (n >= 0) {

        pci_create_simple(pci_bus, -1, "lsi53c895a");

        n--;

    }

    for(n = 0; n < nb_nics; n++) {

        nd = &nd_table[n];



        if ((!nd->model && !done_smc) || strcmp(nd->model, "smc91c111") == 0) {

            smc91c111_init(nd, 0x4e000000, pic[28]);

            done_smc = 1;

        } else {

            pci_nic_init(nd, "rtl8139", NULL);

        }

    }



    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    



    

    ram_offset = qemu_ram_alloc(0x1000);

    cpu_register_physical_memory(0x80000000, 0x1000, ram_offset | IO_MEM_RAM);



    realview_binfo.ram_size = ram_size;

    realview_binfo.kernel_filename = kernel_filename;

    realview_binfo.kernel_cmdline = kernel_cmdline;

    realview_binfo.initrd_filename = initrd_filename;

    realview_binfo.nb_cpus = ncpu;

    arm_load_kernel(first_cpu, &realview_binfo);

}