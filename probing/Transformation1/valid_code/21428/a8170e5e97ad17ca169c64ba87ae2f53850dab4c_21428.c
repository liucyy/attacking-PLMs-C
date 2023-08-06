static void mpcore_priv_map_setup(mpcore_priv_state *s)

{

    int i;

    SysBusDevice *gicbusdev = sysbus_from_qdev(s->gic);

    SysBusDevice *busdev = sysbus_from_qdev(s->mptimer);

    memory_region_init(&s->container, "mpcode-priv-container", 0x2000);

    memory_region_init_io(&s->iomem, &mpcore_scu_ops, s, "mpcore-scu", 0x100);

    memory_region_add_subregion(&s->container, 0, &s->iomem);

    

    for (i = 0; i < (s->num_cpu + 1); i++) {

        target_phys_addr_t offset = 0x100 + (i * 0x100);

        memory_region_add_subregion(&s->container, offset,

                                    sysbus_mmio_get_region(gicbusdev, i + 1));

    }

    

    for (i = 0; i < (s->num_cpu + 1) * 2; i++) {

        

        target_phys_addr_t offset = 0x600 + (i >> 1) * 0x100 + (i & 1) * 0x20;

        memory_region_add_subregion(&s->container, offset,

                                    sysbus_mmio_get_region(busdev, i));

    }

    memory_region_add_subregion(&s->container, 0x1000,

                                sysbus_mmio_get_region(gicbusdev, 0));

    

    for (i = 0; i < s->num_cpu; i++) {

        int ppibase = (s->num_irq - 32) + i * 32;

        sysbus_connect_irq(busdev, i * 2,

                           qdev_get_gpio_in(s->gic, ppibase + 29));

        sysbus_connect_irq(busdev, i * 2 + 1,

                           qdev_get_gpio_in(s->gic, ppibase + 30));

    }

}