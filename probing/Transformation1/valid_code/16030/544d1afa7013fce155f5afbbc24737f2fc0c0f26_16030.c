static void gic_init(gic_state *s, int num_cpu, int num_irq)

#else

static void gic_init(gic_state *s, int num_irq)

#endif

{

    int i;



#if NCPU > 1

    s->num_cpu = num_cpu;

    if (s->num_cpu > NCPU) {

        hw_error("requested %u CPUs exceeds GIC maximum %d\n",

                 num_cpu, NCPU);

    }

#endif

    s->num_irq = num_irq + GIC_BASE_IRQ;

    if (s->num_irq > GIC_MAXIRQ) {

        hw_error("requested %u interrupt lines exceeds GIC maximum %d\n",

                 num_irq, GIC_MAXIRQ);

    }

    

    if (s->num_irq < 32 || (s->num_irq % 32)) {

        hw_error("%d interrupt lines unsupported: not divisible by 32\n",

                 num_irq);

    }



    qdev_init_gpio_in(&s->busdev.qdev, gic_set_irq, s->num_irq - GIC_INTERNAL);

    for (i = 0; i < NUM_CPU(s); i++) {

        sysbus_init_irq(&s->busdev, &s->parent_irq[i]);

    }

    memory_region_init_io(&s->iomem, &gic_dist_ops, s, "gic_dist", 0x1000);

#ifndef NVIC

    

    memory_region_init_io(&s->cpuiomem[0], &gic_thiscpu_ops, s,

                          "gic_cpu", 0x100);

    for (i = 0; i < NUM_CPU(s); i++) {

        s->backref[i] = s;

        memory_region_init_io(&s->cpuiomem[i+1], &gic_cpu_ops, &s->backref[i],

                              "gic_cpu", 0x100);

    }

#endif



    gic_reset(s);

    register_savevm(NULL, "arm_gic", -1, 2, gic_save, gic_load, s);

}