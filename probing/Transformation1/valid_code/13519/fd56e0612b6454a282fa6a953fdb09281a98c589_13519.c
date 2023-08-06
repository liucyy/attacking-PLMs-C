static int pxb_map_irq_fn(PCIDevice *pci_dev, int pin)

{

    PCIDevice *pxb = pci_dev->bus->parent_dev;



    

    return pin - PCI_SLOT(pxb->devfn);

}