int sclp_service_call(uint32_t sccb, uint64_t code)

{

    int r = 0;

    SCCB work_sccb;



    hwaddr sccb_len = sizeof(SCCB);



    

    if (cpu_physical_memory_is_io(sccb)) {

        r = -PGM_ADDRESSING;

        goto out;

    }

    if (sccb & ~0x7ffffff8ul) {

        r = -PGM_SPECIFICATION;

        goto out;

    }



    

    cpu_physical_memory_read(sccb, &work_sccb, sccb_len);



    

    if (be16_to_cpu(work_sccb.h.length) < sizeof(SCCBHeader) ||

        be16_to_cpu(work_sccb.h.length) > SCCB_SIZE) {

        r = -PGM_SPECIFICATION;

        goto out;

    }



    sclp_execute((SCCB *)&work_sccb, code);



    cpu_physical_memory_write(sccb, &work_sccb,

                              be16_to_cpu(work_sccb.h.length));



    sclp_service_interrupt(sccb);



out:

    return r;

}