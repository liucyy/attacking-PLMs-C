static void nvdimm_build_fit(Aml *dev)

{

    Aml *method, *pkg, *buf, *buf_size, *offset, *call_result;

    Aml *whilectx, *ifcond, *ifctx, *elsectx, *fit;



    buf = aml_local(0);

    buf_size = aml_local(1);

    fit = aml_local(2);



    aml_append(dev, aml_create_dword_field(aml_buffer(4, NULL),

               aml_int(0), NVDIMM_DSM_RFIT_STATUS));



    

    method = aml_method("RFIT", 1, AML_SERIALIZED);

    aml_append(method, aml_create_dword_field(aml_buffer(4, NULL),

                                              aml_int(0), "OFST"));



    

    pkg = aml_package(1);

    aml_append(method, aml_store(aml_arg(0), aml_name("OFST")));

    aml_append(pkg, aml_name("OFST"));



    

    call_result = aml_call5(NVDIMM_COMMON_DSM,

                            aml_touuid(NVDIMM_QEMU_RSVD_UUID),

                            aml_int(1) ,

                            aml_int(0x1) ,

                            pkg, aml_int(NVDIMM_QEMU_RSVD_HANDLE_ROOT));

    aml_append(method, aml_store(call_result, buf));



    

    aml_append(method, aml_create_dword_field(buf,

               aml_int(0) , "STAU"));



    aml_append(method, aml_store(aml_name("STAU"),

                                 aml_name(NVDIMM_DSM_RFIT_STATUS)));



     

    ifcond = aml_equal(aml_int(NVDIMM_DSM_RET_STATUS_SUCCESS),

                       aml_name("STAU"));

    ifctx = aml_if(aml_lnot(ifcond));

    aml_append(ifctx, aml_return(aml_buffer(0, NULL)));

    aml_append(method, ifctx);



    aml_append(method, aml_store(aml_sizeof(buf), buf_size));

    aml_append(method, aml_subtract(buf_size,

                                    aml_int(4) ,

                                    buf_size));



    

    ifctx = aml_if(aml_equal(buf_size, aml_int(0)));

    aml_append(ifctx, aml_return(aml_buffer(0, NULL)));

    aml_append(method, ifctx);



    aml_append(method, aml_create_field(buf,

                            aml_int(4 * BITS_PER_BYTE), 

                            aml_shiftleft(buf_size, aml_int(3)), "BUFF"));

    aml_append(method, aml_return(aml_name("BUFF")));

    aml_append(dev, method);



    

    method = aml_method("_FIT", 0, AML_SERIALIZED);

    offset = aml_local(3);



    aml_append(method, aml_store(aml_buffer(0, NULL), fit));

    aml_append(method, aml_store(aml_int(0), offset));



    whilectx = aml_while(aml_int(1));

    aml_append(whilectx, aml_store(aml_call1("RFIT", offset), buf));

    aml_append(whilectx, aml_store(aml_sizeof(buf), buf_size));



    

    ifctx = aml_if(aml_equal(aml_name(NVDIMM_DSM_RFIT_STATUS),

                             aml_int(NVDIMM_DSM_RET_STATUS_FIT_CHANGED)));

    aml_append(ifctx, aml_store(aml_buffer(0, NULL), fit));

    aml_append(ifctx, aml_store(aml_int(0), offset));

    aml_append(whilectx, ifctx);



    elsectx = aml_else();



    

    ifctx = aml_if(aml_equal(buf_size, aml_int(0)));

    aml_append(ifctx, aml_return(fit));

    aml_append(elsectx, ifctx);



    

    aml_append(elsectx, aml_add(offset, buf_size, offset));

    

    aml_append(elsectx, aml_concatenate(fit, buf, fit));

    aml_append(whilectx, elsectx);

    aml_append(method, whilectx);



    aml_append(dev, method);

}