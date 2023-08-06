static uint32_t acpi_find_vgia(void)

{

    uint32_t rsdp_offset;

    uint32_t guid_offset = 0;

    AcpiRsdpDescriptor rsdp_table;

    uint32_t rsdt;

    AcpiRsdtDescriptorRev1 rsdt_table;

    int tables_nr;

    uint32_t *tables;

    AcpiTableHeader ssdt_table;

    VgidTable vgid_table;

    int i;



    

    for (i = 0; i < RSDP_TRIES_MAX; i++) {

        rsdp_offset = acpi_find_rsdp_address();

        if (rsdp_offset < RSDP_ADDR_INVALID) {

            break;

        }

        g_usleep(RSDP_SLEEP_US);

    }

    g_assert_cmphex(rsdp_offset, <, RSDP_ADDR_INVALID);



    acpi_parse_rsdp_table(rsdp_offset, &rsdp_table);



    rsdt = rsdp_table.rsdt_physical_address;

    

    ACPI_READ_TABLE_HEADER(&rsdt_table, rsdt);

    ACPI_ASSERT_CMP(rsdt_table.signature, "RSDT");



    

    tables_nr = (rsdt_table.length - sizeof(AcpiRsdtDescriptorRev1)) /

                sizeof(uint32_t);

    g_assert_cmpint(tables_nr, >, 0);



    

    tables = g_new0(uint32_t, tables_nr);

    ACPI_READ_ARRAY_PTR(tables, tables_nr, rsdt);



    for (i = 0; i < tables_nr; i++) {

        ACPI_READ_TABLE_HEADER(&ssdt_table, tables[i]);

        if (!strncmp((char *)ssdt_table.oem_table_id, "VMGENID", 7)) {

            

            ACPI_READ_FIELD(vgid_table.name_op, tables[i]);

            g_assert(vgid_table.name_op == 0x08);  

            ACPI_READ_ARRAY(vgid_table.vgia, tables[i]);

            g_assert(memcmp(vgid_table.vgia, "VGIA", 4) == 0);

            ACPI_READ_FIELD(vgid_table.val_op, tables[i]);

            g_assert(vgid_table.val_op == 0x0C);  

            ACPI_READ_FIELD(vgid_table.vgia_val, tables[i]);

            

            guid_offset = vgid_table.vgia_val + VMGENID_GUID_OFFSET;

            break;

        }

    }

    g_free(tables);

    return guid_offset;

}