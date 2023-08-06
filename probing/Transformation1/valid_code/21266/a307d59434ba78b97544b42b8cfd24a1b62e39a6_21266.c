qemu_irq spapr_allocate_irq(uint32_t hint, uint32_t *irq_num,

                            enum xics_irq_type type)

{

    uint32_t irq;

    qemu_irq qirq;



    if (hint) {

        irq = hint;

        

    } else {

        irq = spapr->next_irq++;

    }



    qirq = xics_assign_irq(spapr->icp, irq, type);

    if (!qirq) {

        return NULL;

    }



    if (irq_num) {

        *irq_num = irq;

    }



    return qirq;

}