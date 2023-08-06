static bool ept_emulation_fault(uint64_t ept_qual)

{

    int read, write;



    

    if (ept_qual & EPT_VIOLATION_INST_FETCH) {

        return false;

    }



    

    read = ept_qual & EPT_VIOLATION_DATA_READ ? 1 : 0;

    write = ept_qual & EPT_VIOLATION_DATA_WRITE ? 1 : 0;

    if ((read | write) == 0) {

        return false;

    }



    

    if ((ept_qual & EPT_VIOLATION_GLA_VALID) == 0 ||

        (ept_qual & EPT_VIOLATION_XLAT_VALID) == 0) {

        return false;

    }



    return true;

}