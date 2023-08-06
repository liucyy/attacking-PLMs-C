static void create_gic(VirtBoardInfo *vbi, qemu_irq *pic)

{

    

    DeviceState *gicdev;

    SysBusDevice *gicbusdev;

    const char *gictype;

    int i;



    gictype = gic_class_name();



    gicdev = qdev_create(NULL, gictype);

    qdev_prop_set_uint32(gicdev, "revision", 2);

    qdev_prop_set_uint32(gicdev, "num-cpu", smp_cpus);

    

    qdev_prop_set_uint32(gicdev, "num-irq", NUM_IRQS + 32);

    qdev_init_nofail(gicdev);

    gicbusdev = SYS_BUS_DEVICE(gicdev);

    sysbus_mmio_map(gicbusdev, 0, vbi->memmap[VIRT_GIC_DIST].base);

    sysbus_mmio_map(gicbusdev, 1, vbi->memmap[VIRT_GIC_CPU].base);



    

    for (i = 0; i < smp_cpus; i++) {

        DeviceState *cpudev = DEVICE(qemu_get_cpu(i));

        int ppibase = NUM_IRQS + i * GIC_INTERNAL + GIC_NR_SGIS;

        

        qdev_connect_gpio_out(cpudev, 0,

                              qdev_get_gpio_in(gicdev,

                                             ppibase + ARCH_TIMER_NS_EL1_IRQ));

        

        qdev_connect_gpio_out(cpudev, 1,

                              qdev_get_gpio_in(gicdev,

                                               ppibase + ARCH_TIMER_VIRT_IRQ));

        

        qdev_connect_gpio_out(cpudev, 2,

                              qdev_get_gpio_in(gicdev,

                                             ppibase + ARCH_TIMER_NS_EL2_IRQ));



        sysbus_connect_irq(gicbusdev, i, qdev_get_gpio_in(cpudev, ARM_CPU_IRQ));

        sysbus_connect_irq(gicbusdev, i + smp_cpus,

                           qdev_get_gpio_in(cpudev, ARM_CPU_FIQ));

    }



    for (i = 0; i < NUM_IRQS; i++) {

        pic[i] = qdev_get_gpio_in(gicdev, i);

    }



    fdt_add_gic_node(vbi);



    create_v2m(vbi, pic);

}