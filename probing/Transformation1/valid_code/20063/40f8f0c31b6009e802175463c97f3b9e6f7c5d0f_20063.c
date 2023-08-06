static void pcie_aer_msg(PCIDevice *dev, const PCIEAERMsg *msg)

{

    uint8_t type;



    while (dev) {

        if (!pci_is_express(dev)) {

            

            

            return;

        }



        type = pcie_cap_get_type(dev);

        if ((type == PCI_EXP_TYPE_ROOT_PORT ||

            type == PCI_EXP_TYPE_UPSTREAM ||

            type == PCI_EXP_TYPE_DOWNSTREAM) &&

            !pcie_aer_msg_vbridge(dev, msg)) {

                return;

        }

        if (!pcie_aer_msg_alldev(dev, msg)) {

            return;

        }

        if (type == PCI_EXP_TYPE_ROOT_PORT) {

            pcie_aer_msg_root_port(dev, msg);

            

            

            return;

        }

        dev = pci_bridge_get_device(dev->bus);

    }

}