static int usb_ehci_initfn(PCIDevice *dev)

{

    EHCIState *s = DO_UPCAST(EHCIState, dev, dev);

    uint8_t *pci_conf = s->dev.config;

    int i;



    pci_set_byte(&pci_conf[PCI_CLASS_PROG], 0x20);



    

    pci_set_byte(&pci_conf[PCI_CAPABILITY_LIST], 0x00);

    



    pci_set_byte(&pci_conf[PCI_INTERRUPT_PIN], 4); 

    pci_set_byte(&pci_conf[PCI_MIN_GNT], 0);

    pci_set_byte(&pci_conf[PCI_MAX_LAT], 0);



    



    pci_set_byte(&pci_conf[USB_SBRN], USB_RELEASE_2); 

    pci_set_byte(&pci_conf[0x61], 0x20);  

    pci_set_word(&pci_conf[0x62], 0x00);  



    pci_conf[0x64] = 0x00;

    pci_conf[0x65] = 0x00;

    pci_conf[0x66] = 0x00;

    pci_conf[0x67] = 0x00;

    pci_conf[0x68] = 0x01;

    pci_conf[0x69] = 0x00;

    pci_conf[0x6a] = 0x00;

    pci_conf[0x6b] = 0x00;  

    pci_conf[0x6c] = 0x00;

    pci_conf[0x6d] = 0x00;

    pci_conf[0x6e] = 0x00;

    pci_conf[0x6f] = 0xc0;  



    

    s->mmio[0x00] = (uint8_t) OPREGBASE;

    s->mmio[0x01] = 0x00;

    s->mmio[0x02] = 0x00;

    s->mmio[0x03] = 0x01;        

    s->mmio[0x04] = NB_PORTS;    

    s->mmio[0x05] = 0x00;        

    s->mmio[0x06] = 0x00;

    s->mmio[0x07] = 0x00;

    s->mmio[0x08] = 0x80;        

    s->mmio[0x09] = 0x68;        

    s->mmio[0x0a] = 0x00;

    s->mmio[0x0b] = 0x00;



    s->irq = s->dev.irq[3];



    usb_bus_new(&s->bus, &ehci_bus_ops, &s->dev.qdev);

    for(i = 0; i < NB_PORTS; i++) {

        usb_register_port(&s->bus, &s->ports[i], s, i, &ehci_port_ops,

                          USB_SPEED_MASK_HIGH);

        s->ports[i].dev = 0;

    }



    s->frame_timer = qemu_new_timer_ns(vm_clock, ehci_frame_timer, s);

    s->async_bh = qemu_bh_new(ehci_async_bh, s);

    QTAILQ_INIT(&s->aqueues);

    QTAILQ_INIT(&s->pqueues);

    usb_packet_init(&s->ipacket);



    qemu_register_reset(ehci_reset, s);



    memory_region_init_io(&s->mem, &ehci_mem_ops, s, "ehci", MMIO_SIZE);

    pci_register_bar(&s->dev, 0, PCI_BASE_ADDRESS_SPACE_MEMORY, &s->mem);



    return 0;

}