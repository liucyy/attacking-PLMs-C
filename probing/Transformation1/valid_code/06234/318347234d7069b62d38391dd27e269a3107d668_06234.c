static uint32_t set_allocation_state(sPAPRDRConnector *drc,

                                     sPAPRDRAllocationState state)

{

    sPAPRDRConnectorClass *drck = SPAPR_DR_CONNECTOR_GET_CLASS(drc);



    trace_spapr_drc_set_allocation_state(get_index(drc), state);



    if (state == SPAPR_DR_ALLOCATION_STATE_USABLE) {

        

        if (!drc->dev) {

            return RTAS_OUT_NO_SUCH_INDICATOR;

        }

        if (drc->awaiting_release && drc->awaiting_allocation) {

            

            drc->awaiting_allocation_skippable = true;

            return RTAS_OUT_NO_SUCH_INDICATOR;

        }

    }



    if (drc->type != SPAPR_DR_CONNECTOR_TYPE_PCI) {

        drc->allocation_state = state;

        if (drc->awaiting_release &&

            drc->allocation_state == SPAPR_DR_ALLOCATION_STATE_UNUSABLE) {

            trace_spapr_drc_set_allocation_state_finalizing(get_index(drc));

            drck->detach(drc, DEVICE(drc->dev), drc->detach_cb,

                         drc->detach_cb_opaque, NULL);

        } else if (drc->allocation_state == SPAPR_DR_ALLOCATION_STATE_USABLE) {

            drc->awaiting_allocation = false;

        }

    }

    return RTAS_OUT_SUCCESS;

}