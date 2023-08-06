static bool pcie_has_upstream_port(PCIDevice *dev)

{

    PCIDevice *parent_dev = pci_bridge_get_device(dev->bus);



    

    return parent_dev &&

        pci_is_express(parent_dev) &&

        parent_dev->exp.exp_cap &&

        (pcie_cap_get_type(parent_dev) == PCI_EXP_TYPE_ROOT_PORT ||

         pcie_cap_get_type(parent_dev) == PCI_EXP_TYPE_DOWNSTREAM);

}