static int pci_bridge_initfn(PCIDevice *dev)

{

    PCIBridge *s = DO_UPCAST(PCIBridge, dev, dev);



    pci_config_set_vendor_id(s->dev.config, s->vid);

    pci_config_set_device_id(s->dev.config, s->did);



    s->dev.config[0x04] = 0x06; 

    s->dev.config[0x05] = 0x00;

    s->dev.config[0x06] = 0xa0; 

    s->dev.config[0x07] = 0x00; 

    s->dev.config[0x08] = 0x00; 

    s->dev.config[0x09] = 0x00; 

    pci_config_set_class(s->dev.config, PCI_CLASS_BRIDGE_PCI);

    s->dev.config[0x0D] = 0x10; 

    s->dev.config[PCI_HEADER_TYPE] =

        PCI_HEADER_TYPE_MULTI_FUNCTION | PCI_HEADER_TYPE_BRIDGE; 

    s->dev.config[0x1E] = 0xa0; 

    return 0;

}