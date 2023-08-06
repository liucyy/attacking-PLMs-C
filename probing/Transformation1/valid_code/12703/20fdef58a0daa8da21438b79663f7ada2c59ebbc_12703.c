static void nvdimm_build_structure_dcr(GArray *structures, DeviceState *dev)

{

    NvdimmNfitControlRegion *nfit_dcr;

    int slot = object_property_get_int(OBJECT(dev), PC_DIMM_SLOT_PROP,

                                       NULL);

    uint32_t sn = nvdimm_slot_to_sn(slot);



    nfit_dcr = acpi_data_push(structures, sizeof(*nfit_dcr));



    nfit_dcr->type = cpu_to_le16(4 );

    nfit_dcr->length = cpu_to_le16(sizeof(*nfit_dcr));

    nfit_dcr->dcr_index = cpu_to_le16(nvdimm_slot_to_dcr_index(slot));



    

    nfit_dcr->vendor_id = cpu_to_le16(0x8086);

    nfit_dcr->device_id = cpu_to_le16(1);



    

    nfit_dcr->revision_id = cpu_to_le16(1 );

    nfit_dcr->serial_number = cpu_to_le32(sn);

    nfit_dcr->fic = cpu_to_le16(0x201 );

}