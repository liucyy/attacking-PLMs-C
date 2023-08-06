static uint32_t nvdimm_get_max_xfer_label_size(void)

{

    uint32_t max_get_size, max_set_size, dsm_memory_size = 4096;



    

    max_get_size = dsm_memory_size - sizeof(NvdimmFuncGetLabelDataOut);



    

    max_set_size = dsm_memory_size - offsetof(NvdimmDsmIn, arg3) -

                   sizeof(NvdimmFuncSetLabelDataIn);



    return MIN(max_get_size, max_set_size);

}