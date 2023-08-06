void gic_complete_irq(GICState *s, int cpu, int irq)

{

    int update = 0;

    int cm = 1 << cpu;

    DPRINTF("EOI %d\n", irq);

    if (irq >= s->num_irq) {

        

        return;

    }

    if (s->running_irq[cpu] == 1023)

        return; 



    if (s->revision == REV_11MPCORE || s->revision == REV_NVIC) {

        

        if (!GIC_TEST_EDGE_TRIGGER(irq) && GIC_TEST_ENABLED(irq, cm)

            && GIC_TEST_LEVEL(irq, cm) && (GIC_TARGET(irq) & cm) != 0) {

            DPRINTF("Set %d pending mask %x\n", irq, cm);

            GIC_SET_PENDING(irq, cm);

            update = 1;

        }

    }



    if (irq != s->running_irq[cpu]) {

        

        int tmp = s->running_irq[cpu];

        while (s->last_active[tmp][cpu] != 1023) {

            if (s->last_active[tmp][cpu] == irq) {

                s->last_active[tmp][cpu] = s->last_active[irq][cpu];

                break;

            }

            tmp = s->last_active[tmp][cpu];

        }

        if (update) {

            gic_update(s);

        }

    } else {

        

        gic_set_running_irq(s, cpu, s->last_active[s->running_irq[cpu]][cpu]);

    }

}