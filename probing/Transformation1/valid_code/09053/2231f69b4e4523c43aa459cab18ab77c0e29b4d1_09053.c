static void create_gic(VirtBoardInfo *vbi, qemu_irq *pic, int type, bool secure)

{

    

    DeviceState *gicdev;

    SysBusDevice *gicbusdev;

    const char *gictype;

    int i;



    gictype = (type == 3) ? gicv3_class_name() : gic_class_name();



    gicdev = qdev_create(NULL, gictype);

    qdev_prop_set_uint32(gicdev, "revision", type);

    qdev_prop_set_uint32(gicdev, "num-cpu", smp_cpus);

    

    qdev_prop_set_uint32(gicdev, "num-irq", NUM_IRQS + 32);

    if (!kvm_irqchip_in_kernel()) {

        qdev_prop_set_bit(gicdev, "has-security-extensions", secure);

    }

    qdev_init_nofail(gicdev);

    gicbusdev = SYS_BUS_DEVICE(gicdev);

    sysbus_mmio_map(gicbusdev, 0, vbi->memmap[VIRT_GIC_DIST].base);

    if (type == 3) {

        sysbus_mmio_map(gicbusdev, 1, vbi->memmap[VIRT_GIC_REDIST].base);

    } else {

        sysbus_mmio_map(gicbusdev, 1, vbi->memmap[VIRT_GIC_CPU].base);

    }



    

    for (i = 0; i < smp_cpus; i++) {

        DeviceState *cpudev = DEVICE(qemu_get_cpu(i));

        int ppibase = NUM_IRQS + i * GIC_INTERNAL + GIC_NR_SGIS;

        int irq;

        

        const int timer_irq[] = {

            [GTIMER_PHYS] = ARCH_TIMER_NS_EL1_IRQ,

            [GTIMER_VIRT] = ARCH_TIMER_VIRT_IRQ,

            [GTIMER_HYP]  = ARCH_TIMER_NS_EL2_IRQ,

            [GTIMER_SEC]  = ARCH_TIMER_S_EL1_IRQ,

        };



        for (irq = 0; irq < ARRAY_SIZE(timer_irq); irq++) {

            qdev_connect_gpio_out(cpudev, irq,

                                  qdev_get_gpio_in(gicdev,

                                                   ppibase + timer_irq[irq]));

        }



        sysbus_connect_irq(gicbusdev, i, qdev_get_gpio_in(cpudev, ARM_CPU_IRQ));

        sysbus_connect_irq(gicbusdev, i + smp_cpus,

                           qdev_get_gpio_in(cpudev, ARM_CPU_FIQ));

    }



    for (i = 0; i < NUM_IRQS; i++) {

        pic[i] = qdev_get_gpio_in(gicdev, i);

    }



    fdt_add_gic_node(vbi, type);



    if (type == 3) {

        create_its(vbi, gicdev);

    } else {

        create_v2m(vbi, pic);

    }

}