static void platform_fixed_ioport_writew(void *opaque, uint32_t addr, uint32_t val)

{

    PCIXenPlatformState *s = opaque;



    switch (addr) {

    case 0: {

        PCIDevice *pci_dev = PCI_DEVICE(s);

        

        if (val & UNPLUG_ALL_DISKS) {

            DPRINTF("unplug disks\n");

            pci_unplug_disks(pci_dev->bus);

        }

        if (val & UNPLUG_ALL_NICS) {

            DPRINTF("unplug nics\n");

            pci_unplug_nics(pci_dev->bus);

        }

        if (val & UNPLUG_AUX_IDE_DISKS) {

            DPRINTF("unplug auxiliary disks not supported\n");

        }

        break;

    }

    case 2:

        switch (val) {

        case 1:

            DPRINTF("Citrix Windows PV drivers loaded in guest\n");

            break;

        case 0:

            DPRINTF("Guest claimed to be running PV product 0?\n");

            break;

        default:

            DPRINTF("Unknown PV product %d loaded in guest\n", val);

            break;

        }

        s->driver_product_version = val;

        break;

    }

}