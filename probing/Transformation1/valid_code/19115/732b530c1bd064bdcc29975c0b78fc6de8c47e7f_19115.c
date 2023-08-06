static void nvdimm_build_common_dsm(Aml *dev)

{

    Aml *method, *ifctx, *function, *dsm_mem, *unpatched, *result_size;

    uint8_t byte_list[1];



    method = aml_method(NVDIMM_COMMON_DSM, 4, AML_SERIALIZED);

    function = aml_arg(2);

    dsm_mem = aml_name(NVDIMM_ACPI_MEM_ADDR);



    

    unpatched = aml_if(aml_equal(dsm_mem, aml_int(0x0)));



    

    ifctx = aml_if(aml_equal(function, aml_int(0)));

    byte_list[0] = 0 ;

    aml_append(ifctx, aml_return(aml_buffer(1, byte_list)));

    aml_append(unpatched, ifctx);



    

    byte_list[0] = 1 ;

    aml_append(unpatched, aml_return(aml_buffer(1, byte_list)));

    aml_append(method, unpatched);



    

    aml_append(method, aml_store(aml_int(0x0), aml_name("HDLE")));

    aml_append(method, aml_store(aml_arg(1), aml_name("REVS")));

    aml_append(method, aml_store(aml_arg(2), aml_name("FUNC")));



    

    aml_append(method, aml_store(dsm_mem, aml_name("NTFI")));



    result_size = aml_local(1);

    aml_append(method, aml_store(aml_name("RLEN"), result_size));

    aml_append(method, aml_store(aml_shiftleft(result_size, aml_int(3)),

                                 result_size));

    aml_append(method, aml_create_field(aml_name("ODAT"), aml_int(0),

                                        result_size, "OBUF"));

    aml_append(method, aml_concatenate(aml_buffer(0, NULL), aml_name("OBUF"),

                                       aml_arg(6)));

    aml_append(method, aml_return(aml_arg(6)));

    aml_append(dev, method);

}