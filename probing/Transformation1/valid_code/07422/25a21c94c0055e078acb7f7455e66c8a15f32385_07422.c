static int ac97_initfn (PCIDevice *dev)

{

    AC97LinkState *s = DO_UPCAST (AC97LinkState, dev, dev);

    uint8_t *c = s->dev.config;



    

    c[PCI_COMMAND] = 0x00;      

    c[PCI_COMMAND + 1] = 0x00;



    

    c[PCI_STATUS] = PCI_STATUS_FAST_BACK;      

    c[PCI_STATUS + 1] = PCI_STATUS_DEVSEL_MEDIUM >> 8;



    c[PCI_CLASS_PROG] = 0x00;      



    

    

    c[PCI_BASE_ADDRESS_0] = PCI_BASE_ADDRESS_SPACE_IO;

    c[PCI_BASE_ADDRESS_0 + 1] = 0x00;

    c[PCI_BASE_ADDRESS_0 + 2] = 0x00;

    c[PCI_BASE_ADDRESS_0 + 3] = 0x00;



    

      

    c[PCI_BASE_ADDRESS_0 + 4] = PCI_BASE_ADDRESS_SPACE_IO;

    c[PCI_BASE_ADDRESS_0 + 5] = 0x00;

    c[PCI_BASE_ADDRESS_0 + 6] = 0x00;

    c[PCI_BASE_ADDRESS_0 + 7] = 0x00;



    c[PCI_SUBSYSTEM_VENDOR_ID] = 0x86;      

    c[PCI_SUBSYSTEM_VENDOR_ID + 1] = 0x80;



    c[PCI_SUBSYSTEM_ID] = 0x00;      

    c[PCI_SUBSYSTEM_ID + 1] = 0x00;



    c[PCI_INTERRUPT_LINE] = 0x00;      

    c[PCI_INTERRUPT_PIN] = 0x01;      



    memory_region_init_io (&s->io_nam, &ac97_io_nam_ops, s, "ac97-nam", 1024);

    memory_region_init_io (&s->io_nabm, &ac97_io_nabm_ops, s, "ac97-nabm", 256);

    pci_register_bar (&s->dev, 0, PCI_BASE_ADDRESS_SPACE_IO, &s->io_nam);

    pci_register_bar (&s->dev, 1, PCI_BASE_ADDRESS_SPACE_IO, &s->io_nabm);

    qemu_register_reset (ac97_on_reset, s);

    AUD_register_card ("ac97", &s->card);

    ac97_on_reset (s);

    return 0;

}