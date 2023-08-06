static void IRQ_local_pipe(OpenPICState *opp, int n_CPU, int n_IRQ)

{

    IRQDest *dst;

    IRQSource *src;

    int priority;



    dst = &opp->dst[n_CPU];

    src = &opp->src[n_IRQ];



    if (src->output != OPENPIC_OUTPUT_INT) {

        

        src->ivpr |= IVPR_ACTIVITY_MASK;

        DPRINTF("%s: Raise OpenPIC output %d cpu %d irq %d\n",

                __func__, src->output, n_CPU, n_IRQ);

        qemu_irq_raise(opp->dst[n_CPU].irqs[src->output]);

        return;

    }



    priority = IVPR_PRIORITY(src->ivpr);

    if (priority <= dst->ctpr) {

        

        DPRINTF("%s: IRQ %d has too low priority on CPU %d\n",

                __func__, n_IRQ, n_CPU);

        return;

    }

    if (IRQ_testbit(&dst->raised, n_IRQ)) {

        

        DPRINTF("%s: IRQ %d was missed on CPU %d\n",

                __func__, n_IRQ, n_CPU);

        return;

    }

    src->ivpr |= IVPR_ACTIVITY_MASK;

    IRQ_setbit(&dst->raised, n_IRQ);

    if (priority < dst->raised.priority) {

        

        DPRINTF("%s: IRQ %d is hidden by raised IRQ %d on CPU %d\n",

                __func__, n_IRQ, dst->raised.next, n_CPU);

        return;

    }

    IRQ_check(opp, &dst->raised);

    if (IRQ_get_next(opp, &dst->servicing) != -1 &&

        priority <= dst->servicing.priority) {

        DPRINTF("%s: IRQ %d is hidden by servicing IRQ %d on CPU %d\n",

                __func__, n_IRQ, dst->servicing.next, n_CPU);

        

        return;

    }

    DPRINTF("Raise OpenPIC INT output cpu %d irq %d\n", n_CPU, n_IRQ);

    qemu_irq_raise(opp->dst[n_CPU].irqs[OPENPIC_OUTPUT_INT]);

}