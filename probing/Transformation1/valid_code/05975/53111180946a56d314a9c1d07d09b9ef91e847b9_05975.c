static int armv7m_nvic_init(SysBusDevice *dev)

{

    nvic_state *s = NVIC(dev);

    NVICClass *nc = NVIC_GET_CLASS(s);



    

    s->gic.num_cpu = 1;

    

    s->gic.revision = 0xffffffff;

    s->num_irq = s->gic.num_irq;

    nc->parent_init(dev);

    gic_init_irqs_and_distributor(&s->gic, s->num_irq);

    

    memory_region_init(&s->container, "nvic", 0x1000);

    

    memory_region_init_io(&s->sysregmem, &nvic_sysreg_ops, s,

                          "nvic_sysregs", 0x1000);

    memory_region_add_subregion(&s->container, 0, &s->sysregmem);

    

    memory_region_init_alias(&s->gic_iomem_alias, "nvic-gic", &s->gic.iomem,

                             0x100, 0xc00);

    memory_region_add_subregion_overlap(&s->container, 0x100,

                                        &s->gic_iomem_alias, 1);

    

    memory_region_add_subregion(get_system_memory(), 0xe000e000, &s->container);

    s->systick.timer = qemu_new_timer_ns(vm_clock, systick_timer_tick, s);

    return 0;

}