static void acpi_set_pci_info(void)

{

    PCIBus *bus = find_i440fx(); 

    unsigned bsel_alloc = 0;



    if (bus) {

        

        pci_for_each_bus_depth_first(bus, acpi_set_bsel, NULL, &bsel_alloc);

    }

}