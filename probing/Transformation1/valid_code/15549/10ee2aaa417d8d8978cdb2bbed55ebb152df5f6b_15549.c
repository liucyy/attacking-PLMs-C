static void ac97_initfn (PCIDevice *dev)

{

    PCIAC97LinkState *d = DO_UPCAST (PCIAC97LinkState, dev, dev);

    AC97LinkState *s = &d->ac97;

    uint8_t *c = d->dev.config;



    s->pci_dev = &d->dev;

    pci_config_set_vendor_id (c, PCI_VENDOR_ID_INTEL); 

    pci_config_set_device_id (c, PCI_DEVICE_ID_INTEL_82801AA_5); 



    c[0x04] = 0x00;      

    c[0x05] = 0x00;



    c[0x06] = 0x80;      

    c[0x07] = 0x02;



    c[0x08] = 0x01;      

    c[0x09] = 0x00;      

    pci_config_set_class (c, PCI_CLASS_MULTIMEDIA_AUDIO); 

    c[PCI_HEADER_TYPE] = PCI_HEADER_TYPE_NORMAL; 



    c[0x10] = 0x01;      

    c[0x11] = 0x00;

    c[0x12] = 0x00;

    c[0x13] = 0x00;



    c[0x14] = 0x01;      

    c[0x15] = 0x00;

    c[0x16] = 0x00;

    c[0x17] = 0x00;



    c[0x2c] = 0x86;      

    c[0x2d] = 0x80;



    c[0x2e] = 0x00;      

    c[0x2f] = 0x00;



    c[0x3c] = 0x00;      

    c[0x3d] = 0x01;      



    pci_register_bar (&d->dev, 0, 256 * 4, PCI_ADDRESS_SPACE_IO, ac97_map);

    pci_register_bar (&d->dev, 1, 64 * 4, PCI_ADDRESS_SPACE_IO, ac97_map);

    register_savevm ("ac97", 0, 2, ac97_save, ac97_load, s);

    qemu_register_reset (ac97_on_reset, s);

    AUD_register_card ("ac97", &s->card);

    ac97_on_reset (s);

}