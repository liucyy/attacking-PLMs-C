PCIBus *typhoon_init(ram_addr_t ram_size, ISABus **isa_bus,

                     qemu_irq *p_rtc_irq,

                     AlphaCPU *cpus[4], pci_map_irq_fn sys_map_irq)

{

    const uint64_t MB = 1024 * 1024;

    const uint64_t GB = 1024 * MB;

    MemoryRegion *addr_space = get_system_memory();

    DeviceState *dev;

    TyphoonState *s;

    PCIHostState *phb;

    PCIBus *b;

    int i;



    dev = qdev_create(NULL, TYPE_TYPHOON_PCI_HOST_BRIDGE);

    qdev_init_nofail(dev);



    s = TYPHOON_PCI_HOST_BRIDGE(dev);

    phb = PCI_HOST_BRIDGE(dev);



    

    for (i = 0; i < 4; i++) {

        AlphaCPU *cpu = cpus[i];

        s->cchip.cpu[i] = cpu;

        if (cpu != NULL) {

            cpu->alarm_timer = qemu_new_timer_ns(rtc_clock,

                                                 typhoon_alarm_timer,

                                                 (void *)((uintptr_t)s + i));

        }

    }



    *p_rtc_irq = *qemu_allocate_irqs(typhoon_set_timer_irq, s, 1);



    

    memory_region_init_ram(&s->ram_region, OBJECT(s), "ram", ram_size);

    vmstate_register_ram_global(&s->ram_region);

    memory_region_add_subregion(addr_space, 0, &s->ram_region);



    

    



    

    memory_region_init_io(&s->pchip.region, OBJECT(s), &pchip_ops, s, "pchip0",

                          256*MB);

    memory_region_add_subregion(addr_space, 0x80180000000ULL,

                                &s->pchip.region);



    

    memory_region_init_io(&s->cchip.region, OBJECT(s), &cchip_ops, s, "cchip0",

                          256*MB);

    memory_region_add_subregion(addr_space, 0x801a0000000ULL,

                                &s->cchip.region);



    

    memory_region_init_io(&s->dchip_region, OBJECT(s), &dchip_ops, s, "dchip0",

                          256*MB);

    memory_region_add_subregion(addr_space, 0x801b0000000ULL,

                                &s->dchip_region);



    

    memory_region_init(&s->pchip.reg_mem, OBJECT(s), "pci0-mem", 4*GB);

    memory_region_add_subregion(addr_space, 0x80000000000ULL,

                                &s->pchip.reg_mem);



    

    memory_region_init(&s->pchip.reg_io, OBJECT(s), "pci0-io", 32*MB);

    memory_region_add_subregion(addr_space, 0x801fc000000ULL,

                                &s->pchip.reg_io);



    b = pci_register_bus(dev, "pci",

                         typhoon_set_irq, sys_map_irq, s,

                         &s->pchip.reg_mem, &s->pchip.reg_io,

                         0, 64, TYPE_PCI_BUS);

    phb->bus = b;



    

    memory_region_init_io(&s->pchip.reg_iack, OBJECT(s), &alpha_pci_iack_ops,

                          b, "pci0-iack", 64*MB);

    memory_region_add_subregion(addr_space, 0x801f8000000ULL,

                                &s->pchip.reg_iack);



    

    memory_region_init_io(&s->pchip.reg_conf, OBJECT(s), &alpha_pci_conf1_ops,

                          b, "pci0-conf", 16*MB);

    memory_region_add_subregion(addr_space, 0x801fe000000ULL,

                                &s->pchip.reg_conf);



    

    

    

    

    

    



    

    

    {

        qemu_irq isa_pci_irq, *isa_irqs;



        *isa_bus = isa_bus_new(NULL, &s->pchip.reg_io);

        isa_pci_irq = *qemu_allocate_irqs(typhoon_set_isa_irq, s, 1);

        isa_irqs = i8259_init(*isa_bus, isa_pci_irq);

        isa_bus_irqs(*isa_bus, isa_irqs);

    }



    return b;

}