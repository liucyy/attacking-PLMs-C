pci_ebus_init1(PCIDevice *s)

{

    isa_bus_new(&s->qdev);



    s->config[0x04] = 0x06; 

    s->config[0x05] = 0x00;

    s->config[0x06] = 0xa0; 

    s->config[0x07] = 0x03; 

    s->config[0x09] = 0x00; 

    s->config[0x0D] = 0x0a; 



    pci_register_bar(s, 0, 0x1000000, PCI_BASE_ADDRESS_SPACE_MEMORY,

                           ebus_mmio_mapfunc);

    pci_register_bar(s, 1, 0x800000,  PCI_BASE_ADDRESS_SPACE_MEMORY,

                           ebus_mmio_mapfunc);

    return 0;

}