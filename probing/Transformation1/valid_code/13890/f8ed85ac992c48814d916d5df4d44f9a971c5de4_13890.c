static void xen_ram_init(PCMachineState *pcms,

                         ram_addr_t ram_size, MemoryRegion **ram_memory_p)

{

    MemoryRegion *sysmem = get_system_memory();

    ram_addr_t block_len;

    uint64_t user_lowmem = object_property_get_int(qdev_get_machine(),

                                                   PC_MACHINE_MAX_RAM_BELOW_4G,

                                                   &error_abort);



    

    if (HVM_BELOW_4G_RAM_END <= user_lowmem) {

        user_lowmem = HVM_BELOW_4G_RAM_END;

    }



    if (ram_size >= user_lowmem) {

        pcms->above_4g_mem_size = ram_size - user_lowmem;

        pcms->below_4g_mem_size = user_lowmem;

    } else {

        pcms->above_4g_mem_size = 0;

        pcms->below_4g_mem_size = ram_size;

    }

    if (!pcms->above_4g_mem_size) {

        block_len = ram_size;

    } else {

        

        block_len = (1ULL << 32) + pcms->above_4g_mem_size;

    }

    memory_region_init_ram(&ram_memory, NULL, "xen.ram", block_len,

                           &error_abort);

    *ram_memory_p = &ram_memory;

    vmstate_register_ram_global(&ram_memory);



    memory_region_init_alias(&ram_640k, NULL, "xen.ram.640k",

                             &ram_memory, 0, 0xa0000);

    memory_region_add_subregion(sysmem, 0, &ram_640k);

    

    memory_region_init_alias(&ram_lo, NULL, "xen.ram.lo",

                             &ram_memory, 0xc0000,

                             pcms->below_4g_mem_size - 0xc0000);

    memory_region_add_subregion(sysmem, 0xc0000, &ram_lo);

    if (pcms->above_4g_mem_size > 0) {

        memory_region_init_alias(&ram_hi, NULL, "xen.ram.hi",

                                 &ram_memory, 0x100000000ULL,

                                 pcms->above_4g_mem_size);

        memory_region_add_subregion(sysmem, 0x100000000ULL, &ram_hi);

    }

}