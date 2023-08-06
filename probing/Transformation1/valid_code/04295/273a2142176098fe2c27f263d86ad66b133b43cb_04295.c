static void pci_reset(EEPRO100State * s)

{

    uint32_t device = s->device;

    uint8_t *pci_conf = s->pci_dev->config;



    logout("%p\n", s);



    

    pci_config_set_vendor_id(pci_conf, PCI_VENDOR_ID_INTEL);

    

    pci_config_set_device_id(pci_conf, PCI_DEVICE_ID_INTEL_82551IT);

    

    PCI_CONFIG_16(PCI_COMMAND, 0x0000);

    

    PCI_CONFIG_16(PCI_STATUS, 0x2800);

    

    PCI_CONFIG_8(PCI_REVISION_ID, 0x08);

    

    PCI_CONFIG_8(0x09, 0x00);

    pci_config_set_class(pci_conf, PCI_CLASS_NETWORK_ETHERNET);

    

    

    

    

    PCI_CONFIG_8(0x0d, 0x20);   

    

    

#if defined(TARGET_I386)





#endif

#if 0

    

    

    PCI_CONFIG_32(PCI_BASE_ADDRESS_0,

                  PCI_ADDRESS_SPACE_MEM | PCI_ADDRESS_SPACE_MEM_PREFETCH);

    

    PCI_CONFIG_32(PCI_BASE_ADDRESS_1, PCI_ADDRESS_SPACE_IO);

#if 0

    

    PCI_CONFIG_32(PCI_BASE_ADDRESS_2, 0xfffe0000 | PCI_ADDRESS_SPACE_MEM);

#endif

#endif

    

    PCI_CONFIG_32(0x30, 0x00000000);

    

    PCI_CONFIG_8(0x34, 0xdc);

    

    PCI_CONFIG_8(0x3d, 1);      

    

    PCI_CONFIG_8(0x3e, 0x08);

    

    PCI_CONFIG_8(0x3f, 0x18);

    

    PCI_CONFIG_32(0xdc, 0x7e210001);



    switch (device) {

    case i82551:

        

        PCI_CONFIG_8(PCI_REVISION_ID, 0x0f);

        break;

    case i82557B:

        PCI_CONFIG_16(PCI_DEVICE_ID, 0x1229);

        PCI_CONFIG_8(PCI_REVISION_ID, 0x02);

        break;

    case i82557C:

        PCI_CONFIG_16(PCI_DEVICE_ID, 0x1229);

        PCI_CONFIG_8(PCI_REVISION_ID, 0x03);

        break;

    case i82558B:

        PCI_CONFIG_16(PCI_DEVICE_ID, 0x1229);

        PCI_CONFIG_16(PCI_STATUS, 0x2810);

        PCI_CONFIG_8(PCI_REVISION_ID, 0x05);

        break;

    case i82559C:

        PCI_CONFIG_16(PCI_DEVICE_ID, 0x1229);

        PCI_CONFIG_16(PCI_STATUS, 0x2810);

        

        break;

    case i82559ER:

        

        PCI_CONFIG_16(PCI_STATUS, 0x2810);

        PCI_CONFIG_8(PCI_REVISION_ID, 0x09);

        break;

    

    

    default:

        logout("Device %X is undefined!\n", device);

    }



    if (device == i82557C || device == i82558B || device == i82559C) {

        logout("Get device id and revision from EEPROM!!!\n");

    }

}