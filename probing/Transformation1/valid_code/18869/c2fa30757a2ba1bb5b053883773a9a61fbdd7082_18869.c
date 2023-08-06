static void nvdimm_dsm_root(NvdimmDsmIn *in, hwaddr dsm_mem_addr)

{

    

    if (!in->function) {

        nvdimm_dsm_function0(0 , dsm_mem_addr);

        return;

    }



    

    nvdimm_dsm_no_payload(1 , dsm_mem_addr);

}