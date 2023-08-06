PCIBus *pci_pmac_init(qemu_irq *pic)

{

    DeviceState *dev;

    SysBusDevice *s;

    UNINState *d;



    

    

    dev = qdev_create(NULL, "Uni-north main");

    qdev_init_nofail(dev);

    s = sysbus_from_qdev(dev);

    d = FROM_SYSBUS(UNINState, s);

    d->host_state.bus = pci_register_bus(&d->busdev.qdev, "pci",

                                         pci_unin_set_irq, pci_unin_map_irq,

                                         pic, 11 << 3, 4);



    pci_create_simple(d->host_state.bus, 11 << 3, "Uni-north main");



    sysbus_mmio_map(s, 0, 0xf2800000);

    sysbus_mmio_map(s, 1, 0xf2c00000);



    

#if 0

    

    pci_create_simple(d->host_state.bus, 12 << 3, "DEC 21154");

#endif



    

    pci_create_simple(d->host_state.bus, 13 << 3, "Uni-north AGP");



    

#if 0

    

    pci_create_simple(d->host_state.bus, 14 << 3, "Uni-north internal");

#endif



    return d->host_state.bus;

}