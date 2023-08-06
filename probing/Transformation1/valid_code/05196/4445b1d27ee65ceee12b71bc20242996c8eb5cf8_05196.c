void spapr_drc_reset(sPAPRDRConnector *drc)

{

    sPAPRDRConnectorClass *drck = SPAPR_DR_CONNECTOR_GET_CLASS(drc);



    trace_spapr_drc_reset(spapr_drc_index(drc));



    g_free(drc->ccs);

    drc->ccs = NULL;



    

    if (drc->unplug_requested) {

        spapr_drc_release(drc);

    }



    if (drc->dev) {

        

        drc->state = drck->ready_state;

    } else {

        drc->state = drck->empty_state;

    }

}