static uint32_t drc_unisolate_physical(sPAPRDRConnector *drc)

{

    

    if (!drc->dev) {

        return RTAS_OUT_NO_SUCH_INDICATOR;

    }



    drc->isolation_state = SPAPR_DR_ISOLATION_STATE_UNISOLATED;



    return RTAS_OUT_SUCCESS;

}