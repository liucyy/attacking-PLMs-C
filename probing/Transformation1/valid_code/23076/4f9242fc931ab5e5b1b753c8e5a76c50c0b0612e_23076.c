static void reset(DeviceState *d)

{

    sPAPRDRConnector *drc = SPAPR_DR_CONNECTOR(d);

    sPAPRDRConnectorClass *drck = SPAPR_DR_CONNECTOR_GET_CLASS(drc);



    trace_spapr_drc_reset(spapr_drc_index(drc));



    g_free(drc->ccs);

    drc->ccs = NULL;



    

    if (drc->awaiting_release) {

        drck->set_isolation_state(drc, SPAPR_DR_ISOLATION_STATE_ISOLATED);

        

        if (drc->awaiting_release) {

            spapr_drc_detach(drc, DEVICE(drc->dev), NULL);

        }



        

        if (spapr_drc_type(drc) != SPAPR_DR_CONNECTOR_TYPE_PCI &&

            drc->awaiting_release) {

            drck->set_allocation_state(drc, SPAPR_DR_ALLOCATION_STATE_UNUSABLE);

        }

    }

}