static int apb_pci_bridge_initfn(PCIDevice *dev)

{

    int rc;



    rc = pci_bridge_initfn(dev);

    if (rc < 0) {

        return rc;

    }



    pci_config_set_vendor_id(dev->config, PCI_VENDOR_ID_SUN);

    pci_config_set_device_id(dev->config, PCI_DEVICE_ID_SUN_SIMBA);



    

    pci_set_word(dev->config + PCI_COMMAND,

                 PCI_COMMAND_MEMORY);

    pci_set_word(dev->config + PCI_STATUS,

                 PCI_STATUS_FAST_BACK | PCI_STATUS_66MHZ |

                 PCI_STATUS_DEVSEL_MEDIUM);

    pci_set_byte(dev->config + PCI_REVISION_ID, 0x11);

    return 0;

}