static void pci_spapr_set_irq(void *opaque, int irq_num, int level)

{

    

    sPAPRPHBState *phb = opaque;



    qemu_set_irq(phb->lsi_table[irq_num].qirq, level);

}