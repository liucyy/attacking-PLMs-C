static PCIReqIDCache pci_req_id_cache_get(PCIDevice *dev)

{

    PCIDevice *parent;

    PCIReqIDCache cache = {

        .dev = dev,

        .type = PCI_REQ_ID_BDF,

    };



    while (!pci_bus_is_root(dev->bus)) {

        

        parent = dev->bus->parent_dev;

        if (pci_is_express(parent)) {

            if (pcie_cap_get_type(parent) == PCI_EXP_TYPE_PCI_BRIDGE) {

                

                cache.type = PCI_REQ_ID_SECONDARY_BUS;

                cache.dev = dev;

            }

        } else {

            

            cache.type = PCI_REQ_ID_BDF;

            cache.dev = parent;

        }

        dev = parent;

    }



    return cache;

}