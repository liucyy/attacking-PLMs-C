static void versatile_init(MachineState *machine, int board_id)
{
    ObjectClass *cpu_oc;
    Object *cpuobj;
    ARMCPU *cpu;
    MemoryRegion *sysmem = get_system_memory();
    MemoryRegion *ram = g_new(MemoryRegion, 1);
    qemu_irq pic[32];
    qemu_irq sic[32];
    DeviceState *dev, *sysctl;
    SysBusDevice *busdev;
    DeviceState *pl041;
    PCIBus *pci_bus;
    NICInfo *nd;
    I2CBus *i2c;
    int n;
    int done_smc = 0;
    DriveInfo *dinfo;
    if (!machine->cpu_model) {
        machine->cpu_model = "arm926";
    cpu_oc = cpu_class_by_name(TYPE_ARM_CPU, machine->cpu_model);
    if (!cpu_oc) {
        fprintf(stderr, "Unable to find CPU definition\n");
    cpuobj = object_new(object_class_get_name(cpu_oc));
    
    
    memory_region_add_subregion(sysmem, 0, ram);
    sysctl = qdev_create(NULL, "realview_sysctl");
    qdev_prop_set_uint32(sysctl, "sys_id", 0x41007004);
    qdev_prop_set_uint32(sysctl, "proc_id", 0x02000000);
    qdev_init_nofail(sysctl);
    sysbus_mmio_map(SYS_BUS_DEVICE(sysctl), 0, 0x10000000);
    dev = sysbus_create_varargs("pl190", 0x10140000,
                                qdev_get_gpio_in(DEVICE(cpu), ARM_CPU_IRQ),
                                qdev_get_gpio_in(DEVICE(cpu), ARM_CPU_FIQ),
                                NULL);
    for (n = 0; n < 32; n++) {
        pic[n] = qdev_get_gpio_in(dev, n);
    dev = sysbus_create_simple(TYPE_VERSATILE_PB_SIC, 0x10003000, NULL);
    for (n = 0; n < 32; n++) {
        sysbus_connect_irq(SYS_BUS_DEVICE(dev), n, pic[n]);
        sic[n] = qdev_get_gpio_in(dev, n);
    sysbus_create_simple("pl050_keyboard", 0x10006000, sic[3]);
    sysbus_create_simple("pl050_mouse", 0x10007000, sic[4]);
    dev = qdev_create(NULL, "versatile_pci");
    busdev = SYS_BUS_DEVICE(dev);
    qdev_init_nofail(dev);
    sysbus_mmio_map(busdev, 0, 0x10001000); 
    sysbus_mmio_map(busdev, 1, 0x41000000); 
    sysbus_mmio_map(busdev, 2, 0x42000000); 
    sysbus_mmio_map(busdev, 3, 0x43000000); 
    sysbus_mmio_map(busdev, 4, 0x44000000); 
    sysbus_mmio_map(busdev, 5, 0x50000000); 
    sysbus_mmio_map(busdev, 6, 0x60000000); 
    sysbus_connect_irq(busdev, 0, sic[27]);
    sysbus_connect_irq(busdev, 1, sic[28]);
    sysbus_connect_irq(busdev, 2, sic[29]);
    sysbus_connect_irq(busdev, 3, sic[30]);
    pci_bus = (PCIBus *)qdev_get_child_bus(dev, "pci");
    for(n = 0; n < nb_nics; n++) {
        nd = &nd_table[n];
        if (!done_smc && (!nd->model || strcmp(nd->model, "smc91c111") == 0)) {
            smc91c111_init(nd, 0x10010000, sic[25]);
            done_smc = 1;
        } else {
            pci_nic_init_nofail(nd, pci_bus, "rtl8139", NULL);
    if (machine_usb(machine)) {
        pci_create_simple(pci_bus, -1, "pci-ohci");
    n = drive_get_max_bus(IF_SCSI);
    while (n >= 0) {
        pci_create_simple(pci_bus, -1, "lsi53c895a");
        n--;
    pl011_create(0x101f1000, pic[12], serial_hds[0]);
    pl011_create(0x101f2000, pic[13], serial_hds[1]);
    pl011_create(0x101f3000, pic[14], serial_hds[2]);
    pl011_create(0x10009000, sic[6], serial_hds[3]);
    sysbus_create_simple("pl080", 0x10130000, pic[17]);
    sysbus_create_simple("sp804", 0x101e2000, pic[4]);
    sysbus_create_simple("sp804", 0x101e3000, pic[5]);
    sysbus_create_simple("pl061", 0x101e4000, pic[6]);
    sysbus_create_simple("pl061", 0x101e5000, pic[7]);
    sysbus_create_simple("pl061", 0x101e6000, pic[8]);
    sysbus_create_simple("pl061", 0x101e7000, pic[9]);
    
    dev = sysbus_create_simple("pl110_versatile", 0x10120000, pic[16]);
    
    qdev_connect_gpio_out(sysctl, 0, qdev_get_gpio_in(dev, 0));
    sysbus_create_varargs("pl181", 0x10005000, sic[22], sic[1], NULL);
    sysbus_create_varargs("pl181", 0x1000b000, sic[23], sic[2], NULL);
    
    sysbus_create_simple("pl031", 0x101e8000, pic[10]);
    dev = sysbus_create_simple("versatile_i2c", 0x10002000, NULL);
    i2c = (I2CBus *)qdev_get_child_bus(dev, "i2c");
    i2c_create_slave(i2c, "ds1338", 0x68);
    
    pl041 = qdev_create(NULL, "pl041");
    qdev_prop_set_uint32(pl041, "nc_fifo_depth", 512);
    qdev_init_nofail(pl041);
    sysbus_mmio_map(SYS_BUS_DEVICE(pl041), 0, 0x10004000);
    sysbus_connect_irq(SYS_BUS_DEVICE(pl041), 0, sic[24]);
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    dinfo = drive_get(IF_PFLASH, 0, 0);
    if (!pflash_cfi01_register(VERSATILE_FLASH_ADDR, NULL, "versatile.flash",
                          VERSATILE_FLASH_SIZE,
                          dinfo ? blk_by_legacy_dinfo(dinfo) : NULL,
                          VERSATILE_FLASH_SECT_SIZE,
                          VERSATILE_FLASH_SIZE / VERSATILE_FLASH_SECT_SIZE,
                          4, 0x0089, 0x0018, 0x0000, 0x0, 0)) {
        fprintf(stderr, "qemu: Error registering flash memory.\n");
    versatile_binfo.ram_size = machine->ram_size;
    versatile_binfo.kernel_filename = machine->kernel_filename;
    versatile_binfo.kernel_cmdline = machine->kernel_cmdline;
    versatile_binfo.initrd_filename = machine->initrd_filename;
    versatile_binfo.board_id = board_id;
    arm_load_kernel(cpu, &versatile_binfo);