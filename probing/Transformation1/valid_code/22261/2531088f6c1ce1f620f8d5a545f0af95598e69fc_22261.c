uint32_t gic_acknowledge_irq(GICState *s, int cpu, MemTxAttrs attrs)

{

    int ret, irq, src;

    int cm = 1 << cpu;



    

    irq = gic_get_current_pending_irq(s, cpu, attrs);




    if (irq >= GIC_MAXIRQ) {

        DPRINTF("ACK, no pending interrupt or it is hidden: %d\n", irq);

        return irq;

    }



    if (GIC_GET_PRIORITY(irq, cpu) >= s->running_priority[cpu]) {

        DPRINTF("ACK, pending interrupt (%d) has insufficient priority\n", irq);

        return 1023;

    }



    if (s->revision == REV_11MPCORE || s->revision == REV_NVIC) {

        

        GIC_CLEAR_PENDING(irq, GIC_TEST_MODEL(irq) ? ALL_CPU_MASK : cm);

        ret = irq;

    } else {

        if (irq < GIC_NR_SGIS) {

            

            assert(s->sgi_pending[irq][cpu] != 0);

            src = ctz32(s->sgi_pending[irq][cpu]);

            s->sgi_pending[irq][cpu] &= ~(1 << src);

            if (s->sgi_pending[irq][cpu] == 0) {

                GIC_CLEAR_PENDING(irq, GIC_TEST_MODEL(irq) ? ALL_CPU_MASK : cm);

            }

            ret = irq | ((src & 0x7) << 10);

        } else {

            

            GIC_CLEAR_PENDING(irq, GIC_TEST_MODEL(irq) ? ALL_CPU_MASK : cm);

            ret = irq;

        }

    }



    gic_activate_irq(s, cpu, irq);

    gic_update(s);

    DPRINTF("ACK %d\n", irq);

    return ret;

}