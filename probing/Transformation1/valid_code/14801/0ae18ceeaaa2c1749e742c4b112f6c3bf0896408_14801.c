static void versatile_init(ram_addr_t ram_size, int vga_ram_size,

                     const char *boot_device, DisplayState *ds,

                     const char *kernel_filename, const char *kernel_cmdline,

                     const char *initrd_filename, const char *cpu_model,

                     int board_id)

{

    CPUState *env;

    qemu_irq *pic;

    qemu_irq *sic;

    void *scsi_hba;

    PCIBus *pci_bus;

    NICInfo *nd;

    int n;

    int done_smc = 0;

    int index;



    if (!cpu_model)

        cpu_model = "arm926";

    env = cpu_init(cpu_model);

    if (!env) {

        fprintf(stderr, "Unable to find CPU definition\n");

        exit(1);

    }

    

    

    cpu_register_physical_memory(0, ram_size, IO_MEM_RAM);



    arm_sysctl_init(0x10000000, 0x41007004);

    pic = arm_pic_init_cpu(env);

    pic = pl190_init(0x10140000, pic[0], pic[1]);

    sic = vpb_sic_init(0x10003000, pic, 31);

    pl050_init(0x10006000, sic[3], 0);

    pl050_init(0x10007000, sic[4], 1);



    pci_bus = pci_vpb_init(sic, 27, 0);

    

    for(n = 0; n < nb_nics; n++) {

        nd = &nd_table[n];

        if (!nd->model)

            nd->model = done_smc ? "rtl8139" : "smc91c111";

        if (strcmp(nd->model, "smc91c111") == 0) {

            smc91c111_init(nd, 0x10010000, sic[25]);

        } else {

            pci_nic_init(pci_bus, nd, -1);

        }

    }

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



    pl011_init(0x101f1000, pic[12], serial_hds[0], PL011_ARM);

    pl011_init(0x101f2000, pic[13], serial_hds[1], PL011_ARM);

    pl011_init(0x101f3000, pic[14], serial_hds[2], PL011_ARM);

    pl011_init(0x10009000, sic[6], serial_hds[3], PL011_ARM);



    pl080_init(0x10130000, pic[17], 8);

    sp804_init(0x101e2000, pic[4]);

    sp804_init(0x101e3000, pic[5]);



    

    pl110_init(ds, 0x10120000, pic[16], 1);



    index = drive_get_index(IF_SD, 0, 0);

    if (index == -1) {

        fprintf(stderr, "qemu: missing SecureDigital card\n");

        exit(1);

    }



    pl181_init(0x10005000, drives_table[index].bdrv, sic[22], sic[1]);

#if 0

    

    pl181_init(0x1000b000, NULL, sic, 23, 2);

#endif



    

    pl031_init(0x101e8000,pic[10]);



    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    

    



    versatile_binfo.ram_size = ram_size;

    versatile_binfo.kernel_filename = kernel_filename;

    versatile_binfo.kernel_cmdline = kernel_cmdline;

    versatile_binfo.initrd_filename = initrd_filename;

    versatile_binfo.board_id = board_id;

    arm_load_kernel(env, &versatile_binfo);

}