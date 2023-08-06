static void nvdimm_build_ssdt(GSList *device_list, GArray *table_offsets,

                              GArray *table_data, GArray *linker)

{

    Aml *ssdt, *sb_scope, *dev;



    acpi_add_table(table_offsets, table_data);



    ssdt = init_aml_allocator();

    acpi_data_push(ssdt->buf, sizeof(AcpiTableHeader));



    sb_scope = aml_scope("\\_SB");



    dev = aml_device("NVDR");



    

    aml_append(dev, aml_name_decl("_HID", aml_string("ACPI0012")));



    nvdimm_build_common_dsm(dev);

    nvdimm_build_device_dsm(dev);



    nvdimm_build_nvdimm_devices(device_list, dev);



    aml_append(sb_scope, dev);



    aml_append(ssdt, sb_scope);

    

    g_array_append_vals(table_data, ssdt->buf->data, ssdt->buf->len);

    build_header(linker, table_data,

        (void *)(table_data->data + table_data->len - ssdt->buf->len),

        "SSDT", ssdt->buf->len, 1, "NVDIMM");

    free_aml_allocator();

}