static void xen_platform_ioport_writeb(void *opaque, hwaddr addr,

                                       uint64_t val, unsigned int size)

{

    PCIXenPlatformState *s = opaque;

    PCIDevice *pci_dev = PCI_DEVICE(s);



    switch (addr) {

    case 0: 

        platform_fixed_ioport_writeb(opaque, 0, (uint32_t)val);

        break;

    case 4:

        if (val == 1) {

            

            pci_unplug_disks(pci_dev->bus);

            pci_unplug_nics(pci_dev->bus);

        }

        break;

    case 8:

        switch (val) {

        case 1:

            pci_unplug_disks(pci_dev->bus);

            break;

        case 2:

            pci_unplug_nics(pci_dev->bus);

            break;

        default:

            log_writeb(s, (uint32_t)val);

            break;

        }

        break;

    default:

        break;

    }

}