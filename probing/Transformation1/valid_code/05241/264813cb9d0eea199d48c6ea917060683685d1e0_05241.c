void nvdimm_build_acpi(GArray *table_offsets, GArray *table_data,

                       BIOSLinker *linker, AcpiNVDIMMState *state,

                       uint32_t ram_slots)

{

    nvdimm_build_nfit(state, table_offsets, table_data, linker);



    

    if (ram_slots) {

        nvdimm_build_ssdt(table_offsets, table_data, linker, state->dsm_mem,

                          ram_slots);

    }

}