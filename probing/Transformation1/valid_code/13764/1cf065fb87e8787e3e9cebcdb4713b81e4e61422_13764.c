static void s390_init(MachineState *machine)

{

    ram_addr_t my_ram_size = machine->ram_size;

    MemoryRegion *sysmem = get_system_memory();

    MemoryRegion *ram = g_new(MemoryRegion, 1);

    int increment_size = 20;

    void *virtio_region;

    hwaddr virtio_region_len;

    hwaddr virtio_region_start;



    if (machine->ram_slots) {

        error_report("Memory hotplug not supported by the selected machine.");

        exit(EXIT_FAILURE);

    }

    

    while ((my_ram_size >> increment_size) > MAX_STORAGE_INCREMENTS) {

        increment_size++;

    }

    my_ram_size = my_ram_size >> increment_size << increment_size;



    

    ram_size = my_ram_size;



    

    s390_bus = s390_virtio_bus_init(&my_ram_size);

    s390_sclp_init();

    s390_init_ipl_dev(machine->kernel_filename, machine->kernel_cmdline,

                      machine->initrd_filename, ZIPL_FILENAME, false);

    s390_flic_init();



    

    s390_virtio_register_hcalls();



    

    memory_region_init_ram(ram, NULL, "s390.ram", my_ram_size, &error_abort);

    vmstate_register_ram_global(ram);

    memory_region_add_subregion(sysmem, 0, ram);



    

    virtio_region_len = my_ram_size - ram_size;

    virtio_region_start = ram_size;

    virtio_region = cpu_physical_memory_map(virtio_region_start,

                                            &virtio_region_len, true);

    memset(virtio_region, 0, virtio_region_len);

    cpu_physical_memory_unmap(virtio_region, virtio_region_len, 1,

                              virtio_region_len);



    

    s390_skeys_init();



    

    s390_init_cpus(machine->cpu_model);



    

    s390_create_virtio_net((BusState *)s390_bus, "virtio-net-s390");



    

    register_savevm(NULL, "todclock", 0, 1, gtod_save, gtod_load, NULL);

}