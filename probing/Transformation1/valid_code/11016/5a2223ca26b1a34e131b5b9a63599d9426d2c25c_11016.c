int acpi_pcihp_device_hotplug(AcpiPciHpState *s, PCIDevice *dev,

                              PCIHotplugState state)

{

    int slot = PCI_SLOT(dev->devfn);

    int bsel = acpi_pcihp_get_bsel(dev->bus);

    if (bsel < 0) {

        return -1;

    }



    

    if (state == PCI_COLDPLUG_ENABLED) {

        s->acpi_pcihp_pci_status[bsel].device_present |= (1U << slot);

        return 0;

    }



    if (state == PCI_HOTPLUG_ENABLED) {

        enable_device(s, bsel, slot);

    } else {

        disable_device(s, bsel, slot);

    }



    return 0;

}