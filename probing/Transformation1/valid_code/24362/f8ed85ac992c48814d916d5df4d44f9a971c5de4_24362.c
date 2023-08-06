static void a15_daughterboard_init(const VexpressMachineState *vms,

                                   ram_addr_t ram_size,

                                   const char *cpu_model,

                                   qemu_irq *pic)

{

    MemoryRegion *sysmem = get_system_memory();

    MemoryRegion *ram = g_new(MemoryRegion, 1);

    MemoryRegion *sram = g_new(MemoryRegion, 1);



    if (!cpu_model) {

        cpu_model = "cortex-a15";

    }



    {

        

        uint64_t rsz = ram_size;

        if (rsz > (30ULL * 1024 * 1024 * 1024)) {

            fprintf(stderr, "vexpress-a15: cannot model more than 30GB RAM\n");

            exit(1);

        }

    }



    memory_region_allocate_system_memory(ram, NULL, "vexpress.highmem",

                                         ram_size);

    

    memory_region_add_subregion(sysmem, 0x80000000, ram);



    

    init_cpus(cpu_model, "a15mpcore_priv", 0x2c000000, pic, vms->secure);



    



    

    

    

    

    

    

    

    

    memory_region_init_ram(sram, NULL, "vexpress.a15sram", 0x10000,

                           &error_abort);

    vmstate_register_ram_global(sram);

    memory_region_add_subregion(sysmem, 0x2e000000, sram);



    

    

}