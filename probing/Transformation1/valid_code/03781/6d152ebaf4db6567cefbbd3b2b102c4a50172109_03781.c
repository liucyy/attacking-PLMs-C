void virt_acpi_build(VirtGuestInfo *guest_info, AcpiBuildTables *tables)

{

    GArray *table_offsets;

    unsigned dsdt, rsdt;

    VirtAcpiCpuInfo cpuinfo;

    GArray *tables_blob = tables->table_data;



    virt_acpi_get_cpu_info(&cpuinfo);



    table_offsets = g_array_new(false, true ,

                                        sizeof(uint32_t));



    bios_linker_loader_alloc(tables->linker, ACPI_BUILD_TABLE_FILE,

                             64, false );



    



    

    dsdt = tables_blob->len;

    build_dsdt(tables_blob, tables->linker, guest_info);



    

    acpi_add_table(table_offsets, tables_blob);

    build_fadt(tables_blob, tables->linker, dsdt);



    acpi_add_table(table_offsets, tables_blob);

    build_madt(tables_blob, tables->linker, guest_info, &cpuinfo);



    acpi_add_table(table_offsets, tables_blob);

    build_gtdt(tables_blob, tables->linker);



    acpi_add_table(table_offsets, tables_blob);

    build_mcfg(tables_blob, tables->linker, guest_info);



    acpi_add_table(table_offsets, tables_blob);

    build_spcr(tables_blob, tables->linker, guest_info);



    

    rsdt = tables_blob->len;

    build_rsdt(tables_blob, tables->linker, table_offsets);



    

    build_rsdp(tables->rsdp, tables->linker, rsdt);



    

    g_array_free(table_offsets, true);

}