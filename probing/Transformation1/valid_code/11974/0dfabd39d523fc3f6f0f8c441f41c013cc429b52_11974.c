static uint32_t set_isolation_state(sPAPRDRConnector *drc,

                                    sPAPRDRIsolationState state)

{

    trace_spapr_drc_set_isolation_state(spapr_drc_index(drc), state);



    

    if (state == SPAPR_DR_ISOLATION_STATE_ISOLATED) {

        g_free(drc->ccs);

        drc->ccs = NULL;

    }



    if (state == SPAPR_DR_ISOLATION_STATE_UNISOLATED) {

        

        if (!drc->dev ||

            drc->allocation_state == SPAPR_DR_ALLOCATION_STATE_UNUSABLE) {

            return RTAS_OUT_NO_SUCH_INDICATOR;

        }

    }



    

    if (spapr_drc_type(drc) == SPAPR_DR_CONNECTOR_TYPE_LMB) {

        if ((state == SPAPR_DR_ISOLATION_STATE_ISOLATED) &&

             !drc->awaiting_release) {

            return RTAS_OUT_HW_ERROR;

        }

    }



    drc->isolation_state = state;



    if (drc->isolation_state == SPAPR_DR_ISOLATION_STATE_ISOLATED) {

        

        if (drc->awaiting_release) {

            uint32_t drc_index = spapr_drc_index(drc);

            if (drc->configured) {

                trace_spapr_drc_set_isolation_state_finalizing(drc_index);

                spapr_drc_detach(drc, DEVICE(drc->dev), NULL);

            } else {

                trace_spapr_drc_set_isolation_state_deferring(drc_index);

            }

        }

        drc->configured = false;

    }



    return RTAS_OUT_SUCCESS;

}