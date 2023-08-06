static void realview_init(ram_addr_t ram_size, int vga_ram_size,

                     const char *boot_device, DisplayState *ds,

                     const char *kernel_filename, const char *kernel_cmdline,

                     const char *initrd_filename, const char *cpu_model)

{

    CPUState *env;

    qemu_irq *pic;

    void *scsi_hba;

    PCIBus *pci_bus;

    NICInfo *nd;

    int n;

    int done_smc = 0;

    qemu_irq cpu_irq[4];

    int ncpu;

    int index;



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

        pic = arm_pic_init_cpu(env);

        cpu_irq[n] = pic[ARM_PIC_CPU_IRQ];

        if (n > 0) {

            

            env->regs[15] = 0x80000000;

        }

    }



    

    

    cpu_register_physical_memory(0, ram_size, IO_MEM_RAM);



    arm_sysctl_init(0x10000000, 0xc1400400);



    if (ncpu == 1) {

        

        pic = realview_gic_init(0x10040000, cpu_irq[0]);

    } else {

        pic = mpcore_irq_init(cpu_irq);

    }



    pl050_init(0x10006000, pic[20], 0);

    pl050_init(0x10007000, pic[21], 1);



    pl011_init(0x10009000, pic[12], serial_hds[0], PL011_ARM);

    pl011_init(0x1000a000, pic[13], serial_hds[1], PL011_ARM);

    pl011_init(0x1000b000, pic[14], serial_hds[2], PL011_ARM);

    pl011_init(0x1000c000, pic[15], serial_hds[3], PL011_ARM);



    

    pl080_init(0x10030000, pic[24], 2);



    sp804_init(0x10011000, pic[4]);

    sp804_init(0x10012000, pic[5]);



    pl110_init(ds, 0x10020000, pic[23], 1);



    index = drive_get_index(IF_SD, 0, 0);

    if (index == -1) {

        fprintf(stderr, "qemu: missing SecureDigital card\n");

        exit(1);

    }

    pl181_init(0x10005000, drives_table[index].bdrv, pic[17], pic[18]);



    pl031_init(0x10017000, pic[10]);



    pci_bus = pci_vpb_init(pic, 48, 1);

    if (usb_enabled) {

        usb_ohci_init_pci(pci_bus, 3, -1);

    }

    if (drive_get_max_bus(IF_SCSI) > 0) {

        fprintf(stderr, "qemu: too many SCSI bus\n");

        exit(1);

    }

    scsi_hba = lsi_scsi_init(pci_bus, -1);

    for (n = 0; n < LSI_MAX_DEVS; n++) {

        index = drive_get_index(IF_SCSI, 0, n);

        if (index == -1)

            continue;

        lsi_scsi_attach(scsi_hba, drives_table[index].bdrv, n);

    }

    for(n = 0; n < nb_nics; n++) {

        nd = &nd_table[n];

        if (!nd->model)

            nd->model = done_smc ? "rtl8139" : "smc91c111";

        if (strcmp(nd->model, "smc91c111") == 0) {

            smc91c111_init(nd, 0x4e000000, pic[28]);

        } else {

            pci_nic_init(pci_bus, nd, -1);

        }

    }



    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    



    realview_binfo.ram_size = ram_size;

    realview_binfo.kernel_filename = kernel_filename;

    realview_binfo.kernel_cmdline = kernel_cmdline;

    realview_binfo.initrd_filename = initrd_filename;

    realview_binfo.nb_cpus = ncpu;

    arm_load_kernel(first_cpu, &realview_binfo);



    

    cpu_register_physical_memory(0x80000000, 0x1000, IO_MEM_RAM + ram_size);

}