static bool spapr_drc_needed(void *opaque)

{

    sPAPRDRConnector *drc = (sPAPRDRConnector *)opaque;

    sPAPRDRConnectorClass *drck = SPAPR_DR_CONNECTOR_GET_CLASS(drc);



    

    if (!drc->dev) {

        return false;

    }



    

    return (drc->state != drck->ready_state);

}