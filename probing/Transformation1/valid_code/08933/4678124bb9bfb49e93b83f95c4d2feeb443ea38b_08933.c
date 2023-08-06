build_fadt(GArray *table_data, BIOSLinker *linker, AcpiPmInfo *pm,

           unsigned facs, unsigned dsdt,

           const char *oem_id, const char *oem_table_id)

{

    AcpiFadtDescriptorRev1 *fadt = acpi_data_push(table_data, sizeof(*fadt));



    fadt->firmware_ctrl = cpu_to_le32(facs);

    

    bios_linker_loader_add_pointer(linker, ACPI_BUILD_TABLE_FILE,

                                   ACPI_BUILD_TABLE_FILE,

                                   &fadt->firmware_ctrl,

                                   sizeof fadt->firmware_ctrl);



    fadt->dsdt = cpu_to_le32(dsdt);

    

    bios_linker_loader_add_pointer(linker, ACPI_BUILD_TABLE_FILE,

                                   ACPI_BUILD_TABLE_FILE,

                                   &fadt->dsdt,

                                   sizeof fadt->dsdt);



    fadt_setup(fadt, pm);



    build_header(linker, table_data,

                 (void *)fadt, "FACP", sizeof(*fadt), 1, oem_id, oem_table_id);

}