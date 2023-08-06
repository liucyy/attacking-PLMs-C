void acpi_build(AcpiBuildTables *tables, MachineState *machine)

{

    PCMachineState *pcms = PC_MACHINE(machine);

    PCMachineClass *pcmc = PC_MACHINE_GET_CLASS(pcms);

    GArray *table_offsets;

    unsigned facs, dsdt, rsdt, fadt;

    AcpiPmInfo pm;

    AcpiMiscInfo misc;

    AcpiMcfgInfo mcfg;

    Range pci_hole, pci_hole64;

    uint8_t *u;

    size_t aml_len = 0;

    GArray *tables_blob = tables->table_data;

    AcpiSlicOem slic_oem = { .id = NULL, .table_id = NULL };



    acpi_get_pm_info(&pm);

    acpi_get_misc_info(&misc);

    acpi_get_pci_holes(&pci_hole, &pci_hole64);

    acpi_get_slic_oem(&slic_oem);



    table_offsets = g_array_new(false, true ,

                                        sizeof(uint32_t));

    ACPI_BUILD_DPRINTF("init ACPI tables\n");



    bios_linker_loader_alloc(tables->linker,

                             ACPI_BUILD_TABLE_FILE, tables_blob,

                             64 ,

                             false );



    

    facs = tables_blob->len;

    build_facs(tables_blob, tables->linker);



    

    dsdt = tables_blob->len;

    build_dsdt(tables_blob, tables->linker, &pm, &misc,

               &pci_hole, &pci_hole64, machine);



    

    aml_len += tables_blob->len - dsdt;



    

    fadt = tables_blob->len;

    acpi_add_table(table_offsets, tables_blob);

    build_fadt(tables_blob, tables->linker, &pm, facs, dsdt,

               slic_oem.id, slic_oem.table_id);

    aml_len += tables_blob->len - fadt;



    acpi_add_table(table_offsets, tables_blob);

    build_madt(tables_blob, tables->linker, pcms);



    if (misc.has_hpet) {

        acpi_add_table(table_offsets, tables_blob);

        build_hpet(tables_blob, tables->linker);

    }

    if (misc.tpm_version != TPM_VERSION_UNSPEC) {

        acpi_add_table(table_offsets, tables_blob);

        build_tpm_tcpa(tables_blob, tables->linker, tables->tcpalog);



        if (misc.tpm_version == TPM_VERSION_2_0) {

            acpi_add_table(table_offsets, tables_blob);

            build_tpm2(tables_blob, tables->linker);

        }

    }

    if (pcms->numa_nodes) {

        acpi_add_table(table_offsets, tables_blob);

        build_srat(tables_blob, tables->linker, machine);

    }

    if (acpi_get_mcfg(&mcfg)) {

        acpi_add_table(table_offsets, tables_blob);

        build_mcfg_q35(tables_blob, tables->linker, &mcfg);

    }

    if (x86_iommu_get_default()) {

        IommuType IOMMUType = x86_iommu_get_type();

        if (IOMMUType == TYPE_AMD) {

            acpi_add_table(table_offsets, tables_blob);

            build_amd_iommu(tables_blob, tables->linker);

        } else if (IOMMUType == TYPE_INTEL) {

            acpi_add_table(table_offsets, tables_blob);

            build_dmar_q35(tables_blob, tables->linker);

        }

    }

    if (pcms->acpi_nvdimm_state.is_enabled) {

        nvdimm_build_acpi(table_offsets, tables_blob, tables->linker,

                          pcms->acpi_nvdimm_state.dsm_mem, machine->ram_slots);

    }



    

    for (u = acpi_table_first(); u; u = acpi_table_next(u)) {

        unsigned len = acpi_table_len(u);



        acpi_add_table(table_offsets, tables_blob);

        g_array_append_vals(tables_blob, u, len);

    }



    

    rsdt = tables_blob->len;

    build_rsdt(tables_blob, tables->linker, table_offsets,

               slic_oem.id, slic_oem.table_id);



    

    build_rsdp(tables->rsdp, tables->linker, rsdt);



    

    if (pcmc->legacy_acpi_table_size) {

        

        int legacy_aml_len =

            pcmc->legacy_acpi_table_size +

            ACPI_BUILD_LEGACY_CPU_AML_SIZE * max_cpus;

        int legacy_table_size =

            ROUND_UP(tables_blob->len - aml_len + legacy_aml_len,

                     ACPI_BUILD_ALIGN_SIZE);

        if (tables_blob->len > legacy_table_size) {

            

            error_report("Warning: migration may not work.");

        }

        g_array_set_size(tables_blob, legacy_table_size);

    } else {

        

        if (tables_blob->len > ACPI_BUILD_TABLE_SIZE / 2) {

            

            error_report("Warning: ACPI tables are larger than 64k.");

            error_report("Warning: migration may not work.");

            error_report("Warning: please remove CPUs, NUMA nodes, "

                         "memory slots or PCI bridges.");

        }

        acpi_align_size(tables_blob, ACPI_BUILD_TABLE_SIZE);

    }



    acpi_align_size(tables->linker->cmd_blob, ACPI_BUILD_ALIGN_SIZE);



    

    g_array_free(table_offsets, true);

}