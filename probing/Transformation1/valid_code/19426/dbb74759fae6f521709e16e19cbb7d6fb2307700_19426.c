static void machvirt_init(MachineState *machine)

{

    VirtMachineState *vms = VIRT_MACHINE(machine);

    VirtMachineClass *vmc = VIRT_MACHINE_GET_CLASS(machine);

    qemu_irq pic[NUM_IRQS];

    MemoryRegion *sysmem = get_system_memory();

    MemoryRegion *secure_sysmem = NULL;

    int n, virt_max_cpus;

    MemoryRegion *ram = g_new(MemoryRegion, 1);

    const char *cpu_model = machine->cpu_model;

    char **cpustr;

    ObjectClass *oc;

    const char *typename;

    CPUClass *cc;

    Error *err = NULL;

    bool firmware_loaded = bios_name || drive_get(IF_PFLASH, 0, 0);

    uint8_t clustersz;



    if (!cpu_model) {

        cpu_model = "cortex-a15";

    }



    

    if (!vms->gic_version) {

        if (!kvm_enabled()) {

            error_report("gic-version=host requires KVM");

            exit(1);

        }



        vms->gic_version = kvm_arm_vgic_probe();

        if (!vms->gic_version) {

            error_report("Unable to determine GIC version supported by host");

            exit(1);

        }

    }



    

    cpustr = g_strsplit(cpu_model, ",", 2);



    if (!cpuname_valid(cpustr[0])) {

        error_report("mach-virt: CPU %s not supported", cpustr[0]);

        exit(1);

    }



    

    if (vms->secure && firmware_loaded) {

        vms->psci_conduit = QEMU_PSCI_CONDUIT_DISABLED;

    } else if (vms->virt) {

        vms->psci_conduit = QEMU_PSCI_CONDUIT_SMC;

    } else {

        vms->psci_conduit = QEMU_PSCI_CONDUIT_HVC;

    }



    

    if (vms->gic_version == 3) {

        virt_max_cpus = vms->memmap[VIRT_GIC_REDIST].size / 0x20000;

        clustersz = GICV3_TARGETLIST_BITS;

    } else {

        virt_max_cpus = GIC_NCPU;

        clustersz = GIC_TARGETLIST_BITS;

    }



    if (max_cpus > virt_max_cpus) {

        error_report("Number of SMP CPUs requested (%d) exceeds max CPUs "

                     "supported by machine 'mach-virt' (%d)",

                     max_cpus, virt_max_cpus);

        exit(1);

    }



    vms->smp_cpus = smp_cpus;



    if (machine->ram_size > vms->memmap[VIRT_MEM].size) {

        error_report("mach-virt: cannot model more than %dGB RAM", RAMLIMIT_GB);

        exit(1);

    }



    if (vms->virt && kvm_enabled()) {

        error_report("mach-virt: KVM does not support providing "

                     "Virtualization extensions to the guest CPU");

        exit(1);

    }



    if (vms->secure) {

        if (kvm_enabled()) {

            error_report("mach-virt: KVM does not support Security extensions");

            exit(1);

        }



        

        secure_sysmem = g_new(MemoryRegion, 1);

        memory_region_init(secure_sysmem, OBJECT(machine), "secure-memory",

                           UINT64_MAX);

        memory_region_add_subregion_overlap(secure_sysmem, 0, sysmem, -1);

    }



    create_fdt(vms);



    oc = cpu_class_by_name(TYPE_ARM_CPU, cpustr[0]);

    if (!oc) {

        error_report("Unable to find CPU definition");

        exit(1);

    }

    typename = object_class_get_name(oc);



    

    cc = CPU_CLASS(oc);

    cc->parse_features(typename, cpustr[1], &err);

    g_strfreev(cpustr);

    if (err) {

        error_report_err(err);

        exit(1);

    }



    for (n = 0; n < smp_cpus; n++) {

        Object *cpuobj = object_new(typename);

        if (!vmc->disallow_affinity_adjustment) {

            

            uint8_t aff1 = n / clustersz;

            uint8_t aff0 = n % clustersz;

            object_property_set_int(cpuobj, (aff1 << ARM_AFF1_SHIFT) | aff0,

                                    "mp-affinity", NULL);

        }



        if (!vms->secure) {

            object_property_set_bool(cpuobj, false, "has_el3", NULL);

        }



        if (!vms->virt && object_property_find(cpuobj, "has_el2", NULL)) {

            object_property_set_bool(cpuobj, false, "has_el2", NULL);

        }



        if (vms->psci_conduit != QEMU_PSCI_CONDUIT_DISABLED) {

            object_property_set_int(cpuobj, vms->psci_conduit,

                                    "psci-conduit", NULL);



            

            if (n > 0) {

                object_property_set_bool(cpuobj, true,

                                         "start-powered-off", NULL);

            }

        }



        if (vmc->no_pmu && object_property_find(cpuobj, "pmu", NULL)) {

            object_property_set_bool(cpuobj, false, "pmu", NULL);

        }



        if (object_property_find(cpuobj, "reset-cbar", NULL)) {

            object_property_set_int(cpuobj, vms->memmap[VIRT_CPUPERIPHS].base,

                                    "reset-cbar", &error_abort);

        }



        object_property_set_link(cpuobj, OBJECT(sysmem), "memory",

                                 &error_abort);

        if (vms->secure) {

            object_property_set_link(cpuobj, OBJECT(secure_sysmem),

                                     "secure-memory", &error_abort);

        }



        object_property_set_bool(cpuobj, true, "realized", NULL);


    }

    fdt_add_timer_nodes(vms);

    fdt_add_cpu_nodes(vms);

    fdt_add_psci_node(vms);



    memory_region_allocate_system_memory(ram, NULL, "mach-virt.ram",

                                         machine->ram_size);

    memory_region_add_subregion(sysmem, vms->memmap[VIRT_MEM].base, ram);



    create_flash(vms, sysmem, secure_sysmem ? secure_sysmem : sysmem);



    create_gic(vms, pic);



    fdt_add_pmu_nodes(vms);



    create_uart(vms, pic, VIRT_UART, sysmem, serial_hds[0]);



    if (vms->secure) {

        create_secure_ram(vms, secure_sysmem);

        create_uart(vms, pic, VIRT_SECURE_UART, secure_sysmem, serial_hds[1]);

    }



    create_rtc(vms, pic);



    create_pcie(vms, pic);



    create_gpio(vms, pic);



    

    create_virtio_devices(vms, pic);



    vms->fw_cfg = create_fw_cfg(vms, &address_space_memory);

    rom_set_fw(vms->fw_cfg);



    vms->machine_done.notify = virt_machine_done;

    qemu_add_machine_init_done_notifier(&vms->machine_done);



    vms->bootinfo.ram_size = machine->ram_size;

    vms->bootinfo.kernel_filename = machine->kernel_filename;

    vms->bootinfo.kernel_cmdline = machine->kernel_cmdline;

    vms->bootinfo.initrd_filename = machine->initrd_filename;

    vms->bootinfo.nb_cpus = smp_cpus;

    vms->bootinfo.board_id = -1;

    vms->bootinfo.loader_start = vms->memmap[VIRT_MEM].base;

    vms->bootinfo.get_dtb = machvirt_dtb;

    vms->bootinfo.firmware_loaded = firmware_loaded;

    arm_load_kernel(ARM_CPU(first_cpu), &vms->bootinfo);



    

    create_platform_bus(vms, pic);

}