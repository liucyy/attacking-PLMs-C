static void exynos4210_gic_init(Object *obj)

{

    DeviceState *dev = DEVICE(obj);

    Exynos4210GicState *s = EXYNOS4210_GIC(obj);

    SysBusDevice *sbd = SYS_BUS_DEVICE(obj);

    uint32_t i;

    const char cpu_prefix[] = "exynos4210-gic-alias_cpu";

    const char dist_prefix[] = "exynos4210-gic-alias_dist";

    char cpu_alias_name[sizeof(cpu_prefix) + 3];

    char dist_alias_name[sizeof(cpu_prefix) + 3];

    SysBusDevice *busdev;



    s->gic = qdev_create(NULL, "arm_gic");

    qdev_prop_set_uint32(s->gic, "num-cpu", s->num_cpu);

    qdev_prop_set_uint32(s->gic, "num-irq", EXYNOS4210_GIC_NIRQ);

    qdev_init_nofail(s->gic);

    busdev = SYS_BUS_DEVICE(s->gic);



    

    sysbus_pass_irq(sbd, busdev);



    

    qdev_init_gpio_in(dev, exynos4210_gic_set_irq,

                      EXYNOS4210_GIC_NIRQ - 32);



    memory_region_init(&s->cpu_container, obj, "exynos4210-cpu-container",

            EXYNOS4210_EXT_GIC_CPU_REGION_SIZE);

    memory_region_init(&s->dist_container, obj, "exynos4210-dist-container",

            EXYNOS4210_EXT_GIC_DIST_REGION_SIZE);



    for (i = 0; i < s->num_cpu; i++) {

        

        sprintf(cpu_alias_name, "%s%x", cpu_prefix, i);

        memory_region_init_alias(&s->cpu_alias[i], obj,

                                 cpu_alias_name,

                                 sysbus_mmio_get_region(busdev, 1),

                                 0,

                                 EXYNOS4210_GIC_CPU_REGION_SIZE);

        memory_region_add_subregion(&s->cpu_container,

                EXYNOS4210_EXT_GIC_CPU_GET_OFFSET(i), &s->cpu_alias[i]);



        

        sprintf(dist_alias_name, "%s%x", dist_prefix, i);

        memory_region_init_alias(&s->dist_alias[i], obj,

                                 dist_alias_name,

                                 sysbus_mmio_get_region(busdev, 0),

                                 0,

                                 EXYNOS4210_GIC_DIST_REGION_SIZE);

        memory_region_add_subregion(&s->dist_container,

                EXYNOS4210_EXT_GIC_DIST_GET_OFFSET(i), &s->dist_alias[i]);

    }



    sysbus_init_mmio(sbd, &s->cpu_container);

    sysbus_init_mmio(sbd, &s->dist_container);

}