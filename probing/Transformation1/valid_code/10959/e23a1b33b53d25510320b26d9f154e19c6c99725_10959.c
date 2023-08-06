static void apc_init(target_phys_addr_t power_base, qemu_irq cpu_halt)

{

    DeviceState *dev;

    SysBusDevice *s;



    dev = qdev_create(NULL, "apc");

    qdev_init(dev);

    s = sysbus_from_qdev(dev);

    

    sysbus_mmio_map(s, 0, power_base);

    sysbus_connect_irq(s, 0, cpu_halt);

}