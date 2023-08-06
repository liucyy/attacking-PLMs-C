build_dsdt(GArray *table_data, GArray *linker, VirtGuestInfo *guest_info)

{

    Aml *scope, *dsdt;

    const MemMapEntry *memmap = guest_info->memmap;

    const int *irqmap = guest_info->irqmap;



    dsdt = init_aml_allocator();

    

    acpi_data_push(dsdt->buf, sizeof(AcpiTableHeader));



    

    scope = aml_scope("\\_SB");

    acpi_dsdt_add_cpus(scope, guest_info->smp_cpus);

    acpi_dsdt_add_uart(scope, &memmap[VIRT_UART],

                       (irqmap[VIRT_UART] + ARM_SPI_BASE));

    acpi_dsdt_add_flash(scope, &memmap[VIRT_FLASH]);

    acpi_dsdt_add_virtio(scope, &memmap[VIRT_MMIO],

                    (irqmap[VIRT_MMIO] + ARM_SPI_BASE), NUM_VIRTIO_TRANSPORTS);

    acpi_dsdt_add_pci(scope, memmap, (irqmap[VIRT_PCIE] + ARM_SPI_BASE),

                      guest_info->use_highmem);

    acpi_dsdt_add_gpio(scope, &memmap[VIRT_GPIO],

                       (irqmap[VIRT_GPIO] + ARM_SPI_BASE));

    acpi_dsdt_add_power_button(scope);



    aml_append(dsdt, scope);



    

    g_array_append_vals(table_data, dsdt->buf->data, dsdt->buf->len);

    build_header(linker, table_data,

        (void *)(table_data->data + table_data->len - dsdt->buf->len),

        "DSDT", dsdt->buf->len, 2, NULL);

    free_aml_allocator();

}