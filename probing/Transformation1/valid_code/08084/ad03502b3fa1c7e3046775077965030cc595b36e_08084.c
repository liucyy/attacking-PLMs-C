static void e100_pci_reset(EEPRO100State * s, E100PCIDeviceInfo *e100_device)

{

    uint32_t device = s->device;

    uint8_t *pci_conf = s->dev.config;



    TRACE(OTHER, logout("%p\n", s));



    

    pci_config_set_vendor_id(pci_conf, PCI_VENDOR_ID_INTEL);

    

    pci_config_set_device_id(pci_conf, e100_device->device_id);

    

    pci_set_word(pci_conf + PCI_STATUS, PCI_STATUS_DEVSEL_MEDIUM |

                                        PCI_STATUS_FAST_BACK);

    

    pci_config_set_revision(pci_conf, e100_device->revision);

    pci_config_set_class(pci_conf, PCI_CLASS_NETWORK_ETHERNET);

    

    pci_set_byte(pci_conf + PCI_LATENCY_TIMER, 0x20);   

    

    

    

    pci_set_byte(pci_conf + PCI_INTERRUPT_PIN, 1);      

    

    pci_set_byte(pci_conf + PCI_MIN_GNT, 0x08);

    

    pci_set_byte(pci_conf + PCI_MAX_LAT, 0x18);



    s->stats_size = e100_device->stats_size;

    s->has_extended_tcb_support = e100_device->has_extended_tcb_support;



    switch (device) {

    case i82550:

    case i82551:

    case i82557A:

    case i82557B:

    case i82557C:

    case i82558A:

    case i82558B:

    case i82559A:

    case i82559B:

    case i82559ER:

    case i82562:

    case i82801:

        break;

    case i82559C:

#if EEPROM_SIZE > 0

        pci_set_word(pci_conf + PCI_SUBSYSTEM_VENDOR_ID, PCI_VENDOR_ID_INTEL);

        pci_set_word(pci_conf + PCI_SUBSYSTEM_ID, 0x0040);

#endif

        break;

    default:

        logout("Device %X is undefined!\n", device);

    }



    

    s->configuration[6] |= BIT(4);



    

    s->configuration[6] |= BIT(5);



    if (s->stats_size == 80) {

        

        if (s->configuration[6] & BIT(2)) {

            

            assert(s->configuration[6] & BIT(5));

        } else {

            if (s->configuration[6] & BIT(5)) {

                

                s->stats_size = 64;

            } else {

                

                s->stats_size = 76;

            }

        }

    } else {

        if (s->configuration[6] & BIT(5)) {

            

            s->stats_size = 64;

        }

    }

    assert(s->stats_size > 0 && s->stats_size <= sizeof(s->statistics));



    if (e100_device->power_management) {

        

        int cfg_offset = 0xdc;

        int r = pci_add_capability(&s->dev, PCI_CAP_ID_PM,

                                   cfg_offset, PCI_PM_SIZEOF);

        assert(r >= 0);

        pci_set_word(pci_conf + cfg_offset + PCI_PM_PMC, 0x7e21);

#if 0 

        

        pci_set_word(pci_conf + cfg_offset + PCI_PM_CTRL, 0x0000);

        

        pci_set_byte(pci_conf + cfg_offset + PCI_PM_PPB_EXTENSIONS, 0x0000);

#endif

    }



#if EEPROM_SIZE > 0

    if (device == i82557C || device == i82558B || device == i82559C) {

        

        logout("Get device id and revision from EEPROM!!!\n");

    }

#endif 

}