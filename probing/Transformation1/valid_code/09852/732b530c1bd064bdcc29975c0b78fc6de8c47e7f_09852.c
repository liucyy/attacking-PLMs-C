static void nvdimm_build_nvdimm_devices(GSList *device_list, Aml *root_dev)

{

    for (; device_list; device_list = device_list->next) {

        DeviceState *dev = device_list->data;

        int slot = object_property_get_int(OBJECT(dev), PC_DIMM_SLOT_PROP,

                                           NULL);

        uint32_t handle = nvdimm_slot_to_handle(slot);

        Aml *nvdimm_dev;



        nvdimm_dev = aml_device("NV%02X", slot);



        

        aml_append(nvdimm_dev, aml_name_decl("_ADR", aml_int(handle)));



        nvdimm_build_device_dsm(nvdimm_dev);

        aml_append(root_dev, nvdimm_dev);

    }

}