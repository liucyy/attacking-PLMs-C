build_mcfg_q35(GArray *table_data, GArray *linker, AcpiMcfgInfo *info)

{

    AcpiTableMcfg *mcfg;

    const char *sig;

    int len = sizeof(*mcfg) + 1 * sizeof(mcfg->allocation[0]);



    mcfg = acpi_data_push(table_data, len);

    mcfg->allocation[0].address = cpu_to_le64(info->mcfg_base);

    

    mcfg->allocation[0].pci_segment = cpu_to_le16(0);

    mcfg->allocation[0].start_bus_number = 0;

    mcfg->allocation[0].end_bus_number = PCIE_MMCFG_BUS(info->mcfg_size - 1);



    

    if (info->mcfg_base == PCIE_BASE_ADDR_UNMAPPED) {

        

        sig = "QEMU";

    } else {

        sig = "MCFG";

    }

    build_header(linker, table_data, (void *)mcfg, sig, len, 1, NULL, NULL);

}