static void ppc_powernv_init(MachineState *machine)

{

    PnvMachineState *pnv = POWERNV_MACHINE(machine);

    MemoryRegion *ram;

    char *fw_filename;

    long fw_size;

    int i;

    char *chip_typename;



    

    if (machine->ram_size < (1 * G_BYTE)) {

        error_report("Warning: skiboot may not work with < 1GB of RAM");

    }



    ram = g_new(MemoryRegion, 1);

    memory_region_allocate_system_memory(ram, NULL, "ppc_powernv.ram",

                                         machine->ram_size);

    memory_region_add_subregion(get_system_memory(), 0, ram);



    

    if (bios_name == NULL) {

        bios_name = FW_FILE_NAME;

    }



    fw_filename = qemu_find_file(QEMU_FILE_TYPE_BIOS, bios_name);



    fw_size = load_image_targphys(fw_filename, FW_LOAD_ADDR, FW_MAX_SIZE);

    if (fw_size < 0) {

        error_report("Could not load OPAL '%s'", fw_filename);

        exit(1);

    }

    g_free(fw_filename);



    

    if (machine->kernel_filename) {

        long kernel_size;



        kernel_size = load_image_targphys(machine->kernel_filename,

                                          KERNEL_LOAD_ADDR, 0x2000000);

        if (kernel_size < 0) {

            error_report("Could not load kernel '%s'",

                         machine->kernel_filename);

            exit(1);

        }

    }



    

    if (machine->initrd_filename) {

        pnv->initrd_base = INITRD_LOAD_ADDR;

        pnv->initrd_size = load_image_targphys(machine->initrd_filename,

                                  pnv->initrd_base, 0x10000000); 

        if (pnv->initrd_size < 0) {

            error_report("Could not load initial ram disk '%s'",

                         machine->initrd_filename);

            exit(1);

        }

    }



    

    if (machine->cpu_model == NULL) {

        machine->cpu_model = "POWER8";

    }



    

    chip_typename = g_strdup_printf(TYPE_PNV_CHIP "-%s", machine->cpu_model);

    if (!object_class_by_name(chip_typename)) {

        error_report("invalid CPU model '%s' for %s machine",

                     machine->cpu_model, MACHINE_GET_CLASS(machine)->name);

        exit(1);

    }



    pnv->chips = g_new0(PnvChip *, pnv->num_chips);

    for (i = 0; i < pnv->num_chips; i++) {

        char chip_name[32];

        Object *chip = object_new(chip_typename);



        pnv->chips[i] = PNV_CHIP(chip);



        

        if (i == 0) {

            object_property_set_int(chip, machine->ram_size, "ram-size",

                                    &error_fatal);

        }



        snprintf(chip_name, sizeof(chip_name), "chip[%d]", PNV_CHIP_HWID(i));

        object_property_add_child(OBJECT(pnv), chip_name, chip, &error_fatal);

        object_property_set_int(chip, PNV_CHIP_HWID(i), "chip-id",

                                &error_fatal);

        object_property_set_int(chip, smp_cores, "nr-cores", &error_fatal);

        object_property_set_bool(chip, true, "realized", &error_fatal);

    }

    g_free(chip_typename);



    

    pnv->isa_bus = pnv_isa_create(pnv->chips[0]);



    

    serial_hds_isa_init(pnv->isa_bus, 0, MAX_SERIAL_PORTS);



    

    rtc_init(pnv->isa_bus, 2000, NULL);



    

    pnv->powerdown_notifier.notify = pnv_powerdown_notify;

    qemu_register_powerdown_notifier(&pnv->powerdown_notifier);

}