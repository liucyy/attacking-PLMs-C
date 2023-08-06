nvdimm_dsm_write(void *opaque, hwaddr addr, uint64_t val, unsigned size)

{

    AcpiNVDIMMState *state = opaque;

    NvdimmDsmIn *in;

    hwaddr dsm_mem_addr = val;



    nvdimm_debug("dsm memory address %#" HWADDR_PRIx ".\n", dsm_mem_addr);



    

    in = g_new(NvdimmDsmIn, 1);

    cpu_physical_memory_read(dsm_mem_addr, in, sizeof(*in));



    le32_to_cpus(&in->revision);

    le32_to_cpus(&in->function);

    le32_to_cpus(&in->handle);



    nvdimm_debug("Revision %#x Handler %#x Function %#x.\n", in->revision,

                 in->handle, in->function);



    if (in->revision != 0x1 ) {

        nvdimm_debug("Revision %#x is not supported, expect %#x.\n",

                     in->revision, 0x1);

        nvdimm_dsm_no_payload(1 , dsm_mem_addr);

        goto exit;

    }



    if (in->handle == NVDIMM_QEMU_RSVD_HANDLE_ROOT) {

        nvdimm_dsm_reserved_root(state, in, dsm_mem_addr);

        goto exit;

    }



     

    if (!in->handle) {

        nvdimm_dsm_root(in, dsm_mem_addr);

        goto exit;

    }



    nvdimm_dsm_device(in, dsm_mem_addr);



exit:

    g_free(in);

}