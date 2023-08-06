PCIDevice *pci_get_function_0(PCIDevice *pci_dev)

{

    if(pcie_has_upstream_port(pci_dev)) {

        

        return pci_dev->bus->devices[0];

    } else {

        

        return pci_dev->bus->devices[PCI_DEVFN(PCI_SLOT(pci_dev->devfn), 0)];

    }

}