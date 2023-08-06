static int pci_init_multifunction(PCIBus *bus, PCIDevice *dev)

{

    uint8_t slot = PCI_SLOT(dev->devfn);

    uint8_t func;



    if (dev->cap_present & QEMU_PCI_CAP_MULTIFUNCTION) {

        dev->config[PCI_HEADER_TYPE] |= PCI_HEADER_TYPE_MULTI_FUNCTION;

    }



    

    if (PCI_FUNC(dev->devfn)) {

        PCIDevice *f0 = bus->devices[PCI_DEVFN(slot, 0)];

        if (f0 && !(f0->cap_present & QEMU_PCI_CAP_MULTIFUNCTION)) {

            

            error_report("PCI: single function device can't be populated "

                         "in function %x.%x", slot, PCI_FUNC(dev->devfn));

            return -1;

        }

        return 0;

    }



    if (dev->cap_present & QEMU_PCI_CAP_MULTIFUNCTION) {

        return 0;

    }

    

    for (func = 1; func < PCI_FUNC_MAX; ++func) {

        if (bus->devices[PCI_DEVFN(slot, func)]) {

            error_report("PCI: %x.0 indicates single function, "

                         "but %x.%x is already populated.",

                         slot, slot, func);

            return -1;

        }

    }

    return 0;

}