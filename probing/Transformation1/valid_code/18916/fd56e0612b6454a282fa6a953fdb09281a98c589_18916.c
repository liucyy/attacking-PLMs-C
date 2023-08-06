static int pci_vpb_map_irq(PCIDevice *d, int irq_num)

{

    PCIVPBState *s = container_of(d->bus, PCIVPBState, pci_bus);



    if (s->irq_mapping == PCI_VPB_IRQMAP_BROKEN) {

        

        return irq_num;

    }



    

    return pci_swizzle_map_irq_fn(d, irq_num + 2);

}