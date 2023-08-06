void acpi_build(PcGuestInfo *guest_info, AcpiBuildTables *tables)

{

    GArray *table_offsets;

    unsigned facs, ssdt, dsdt, rsdt;

    AcpiCpuInfo cpu;

    AcpiPmInfo pm;

    AcpiMiscInfo misc;

    AcpiMcfgInfo mcfg;

    PcPciInfo pci;

    uint8_t *u;

    size_t aml_len = 0;



    acpi_get_cpu_info(&cpu);

    acpi_get_pm_info(&pm);

    acpi_get_dsdt(&misc);

    acpi_get_misc_info(&misc);

    acpi_get_pci_info(&pci);



    table_offsets = g_array_new(false, true ,

                                        sizeof(uint32_t));

    ACPI_BUILD_DPRINTF(3, "init ACPI tables\n");



    bios_linker_loader_alloc(tables->linker, ACPI_BUILD_TABLE_FILE,

                             64 ,

                             false );



    

    facs = tables->table_data->len;

    build_facs(tables->table_data, tables->linker, guest_info);



    

    dsdt = tables->table_data->len;

    build_dsdt(tables->table_data, tables->linker, &misc);



    

    aml_len += tables->table_data->len - dsdt;



    

    acpi_add_table(table_offsets, tables->table_data);

    build_fadt(tables->table_data, tables->linker, &pm, facs, dsdt);



    ssdt = tables->table_data->len;

    acpi_add_table(table_offsets, tables->table_data);

    build_ssdt(tables->table_data, tables->linker, &cpu, &pm, &misc, &pci,

               guest_info);

    aml_len += tables->table_data->len - ssdt;



    acpi_add_table(table_offsets, tables->table_data);

    build_madt(tables->table_data, tables->linker, &cpu, guest_info);



    if (misc.has_hpet) {

        acpi_add_table(table_offsets, tables->table_data);

        build_hpet(tables->table_data, tables->linker);

    }

    if (guest_info->numa_nodes) {

        acpi_add_table(table_offsets, tables->table_data);

        build_srat(tables->table_data, tables->linker, &cpu, guest_info);

    }

    if (acpi_get_mcfg(&mcfg)) {

        acpi_add_table(table_offsets, tables->table_data);

        build_mcfg_q35(tables->table_data, tables->linker, &mcfg);

    }



    

    for (u = acpi_table_first(); u; u = acpi_table_next(u)) {

        unsigned len = acpi_table_len(u);



        acpi_add_table(table_offsets, tables->table_data);

        g_array_append_vals(tables->table_data, u, len);

    }



    

    rsdt = tables->table_data->len;

    build_rsdt(tables->table_data, tables->linker, table_offsets);



    

    build_rsdp(tables->rsdp, tables->linker, rsdt);



    

    if (guest_info->legacy_acpi_table_size) {

        

        int legacy_aml_len =

            guest_info->legacy_acpi_table_size +

            ACPI_BUILD_LEGACY_CPU_AML_SIZE * max_cpus;

        int legacy_table_size =

            ROUND_UP(tables->table_data->len - aml_len + legacy_aml_len,

                     ACPI_BUILD_ALIGN_SIZE);

        if (tables->table_data->len > legacy_table_size) {

            

            error_report("Warning: migration to QEMU 2.0 may not work.");

        }

        g_array_set_size(tables->table_data, legacy_table_size);

    } else {

        if (tables->table_data->len > ACPI_BUILD_TABLE_SIZE) {

            

            error_report("ACPI tables are larger than 64k.  Please remove");

            error_report("CPUs, NUMA nodes, memory slots or PCI bridges.");

            exit(1);

        }

        g_array_set_size(tables->table_data, ACPI_BUILD_TABLE_SIZE);

    }



    acpi_align_size(tables->linker, ACPI_BUILD_ALIGN_SIZE);



    

    g_array_free(table_offsets, true);

}