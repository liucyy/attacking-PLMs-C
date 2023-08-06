static uint32_t drc_isolate_logical(sPAPRDRConnector *drc)

{

    

    g_free(drc->ccs);

    drc->ccs = NULL;



    

    if (spapr_drc_type(drc) == SPAPR_DR_CONNECTOR_TYPE_LMB

        && !drc->unplug_requested) {

        return RTAS_OUT_HW_ERROR;

    }



    drc->isolation_state = SPAPR_DR_ISOLATION_STATE_ISOLATED;



    

    if (drc->unplug_requested) {

        uint32_t drc_index = spapr_drc_index(drc);

        if (drc->configured) {

            trace_spapr_drc_set_isolation_state_finalizing(drc_index);

            spapr_drc_detach(drc);

        } else {

            trace_spapr_drc_set_isolation_state_deferring(drc_index);

        }

    }

    drc->configured = false;



    return RTAS_OUT_SUCCESS;

}