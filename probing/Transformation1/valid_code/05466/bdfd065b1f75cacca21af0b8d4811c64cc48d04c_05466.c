static void nvdimm_build_ssdt(GSList *device_list, GArray *table_offsets,

                              GArray *table_data, BIOSLinker *linker,

                              GArray *dsm_dma_arrea)

{

    Aml *ssdt, *sb_scope, *dev;

    int mem_addr_offset, nvdimm_ssdt;



    acpi_add_table(table_offsets, table_data);



    ssdt = init_aml_allocator();

    acpi_data_push(ssdt->buf, sizeof(AcpiTableHeader));



    sb_scope = aml_scope("\\_SB");



    dev = aml_device("NVDR");



    

    aml_append(dev, aml_name_decl("_HID", aml_string("ACPI0012")));



    nvdimm_build_common_dsm(dev);



    

    nvdimm_build_device_dsm(dev, 0);



    nvdimm_build_nvdimm_devices(device_list, dev);



    aml_append(sb_scope, dev);

    aml_append(ssdt, sb_scope);



    nvdimm_ssdt = table_data->len;



    

    g_array_append_vals(table_data, ssdt->buf->data, ssdt->buf->len);

    mem_addr_offset = build_append_named_dword(table_data,

                                               NVDIMM_ACPI_MEM_ADDR);



    bios_linker_loader_alloc(linker,

                             NVDIMM_DSM_MEM_FILE, dsm_dma_arrea,

                             sizeof(NvdimmDsmIn), false );

    bios_linker_loader_add_pointer(linker,

        ACPI_BUILD_TABLE_FILE, mem_addr_offset, sizeof(uint32_t),

        NVDIMM_DSM_MEM_FILE, 0);

    build_header(linker, table_data,

        (void *)(table_data->data + nvdimm_ssdt),

        "SSDT", table_data->len - nvdimm_ssdt, 1, NULL, "NVDIMM");

    free_aml_allocator();

}