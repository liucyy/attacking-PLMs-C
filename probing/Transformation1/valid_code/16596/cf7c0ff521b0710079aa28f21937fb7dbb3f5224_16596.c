static GArray *nvdimm_build_device_structure(void)

{

    GSList *device_list = nvdimm_get_plugged_device_list();

    GArray *structures = g_array_new(false, true , 1);



    for (; device_list; device_list = device_list->next) {

        DeviceState *dev = device_list->data;



        

        nvdimm_build_structure_spa(structures, dev);



        

        nvdimm_build_structure_memdev(structures, dev);



        

        nvdimm_build_structure_dcr(structures, dev);

    }

    g_slist_free(device_list);



    return structures;

}