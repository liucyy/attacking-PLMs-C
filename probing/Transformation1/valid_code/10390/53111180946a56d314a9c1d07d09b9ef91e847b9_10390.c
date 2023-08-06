static int arm_gic_init(SysBusDevice *dev)

{

    

    int i;

    GICState *s = FROM_SYSBUS(GICState, dev);

    ARMGICClass *agc = ARM_GIC_GET_CLASS(s);



    agc->parent_init(dev);



    gic_init_irqs_and_distributor(s, s->num_irq);



    

    memory_region_init_io(&s->cpuiomem[0], &gic_thiscpu_ops, s,

                          "gic_cpu", 0x100);

    for (i = 0; i < NUM_CPU(s); i++) {

        s->backref[i] = s;

        memory_region_init_io(&s->cpuiomem[i+1], &gic_cpu_ops, &s->backref[i],

                              "gic_cpu", 0x100);

    }

    

    sysbus_init_mmio(dev, &s->iomem);

    

    for (i = 0; i <= NUM_CPU(s); i++) {

        sysbus_init_mmio(dev, &s->cpuiomem[i]);

    }

    return 0;

}