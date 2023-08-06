uint32_t gic_acknowledge_irq(GICState *s, int cpu)

{

    int ret, irq, src;

    int cm = 1 << cpu;

    irq = s->current_pending[cpu];

    if (irq == 1023

            || GIC_GET_PRIORITY(irq, cpu) >= s->running_priority[cpu]) {

        DPRINTF("ACK no pending IRQ\n");

        return 1023;

    }

    s->last_active[irq][cpu] = s->running_irq[cpu];



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



    gic_set_running_irq(s, cpu, irq);

    DPRINTF("ACK %d\n", irq);

    return ret;

}