static uint32_t drc_unisolate_logical(sPAPRDRConnector *drc)

{

    

    if (!drc->dev ||

        drc->allocation_state == SPAPR_DR_ALLOCATION_STATE_UNUSABLE) {

        return RTAS_OUT_NO_SUCH_INDICATOR;

    }



    drc->isolation_state = SPAPR_DR_ISOLATION_STATE_UNISOLATED;



    return RTAS_OUT_SUCCESS;

}