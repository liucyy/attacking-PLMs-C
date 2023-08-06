PCIBus *pci_find_bus(PCIBus *bus, int bus_num)

{

    PCIBus *sec;



    if (!bus)

        return NULL;



    if (pci_bus_num(bus) == bus_num) {

        return bus;

    }



    

    QLIST_FOREACH(sec, &bus->child, sibling) {



        if (!bus->parent_dev 

            || (pci_bus_num(sec) <= bus_num &&

                bus->parent_dev->config[PCI_SUBORDINATE_BUS])) {

            return pci_find_bus(sec, bus_num);

        }

    }



    return NULL;

}