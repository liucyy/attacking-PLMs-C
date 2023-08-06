static uint32_t drc_set_usable(sPAPRDRConnector *drc)

{

    

    if (!drc->dev) {

        return RTAS_OUT_NO_SUCH_INDICATOR;

    }

    if (drc->awaiting_release && drc->awaiting_allocation) {

        

        return RTAS_OUT_NO_SUCH_INDICATOR;

    }



    drc->allocation_state = SPAPR_DR_ALLOCATION_STATE_USABLE;

    drc->awaiting_allocation = false;



    return RTAS_OUT_SUCCESS;

}