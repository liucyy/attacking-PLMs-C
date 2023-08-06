void spapr_drc_detach(sPAPRDRConnector *drc, DeviceState *d, Error **errp)

{

    trace_spapr_drc_detach(spapr_drc_index(drc));



    

    if (!drc->signalled && !drc->configured) {

        

        drc->isolation_state = SPAPR_DR_ISOLATION_STATE_ISOLATED;

    }



    if (drc->isolation_state != SPAPR_DR_ISOLATION_STATE_ISOLATED) {

        trace_spapr_drc_awaiting_isolated(spapr_drc_index(drc));

        drc->awaiting_release = true;

        return;

    }



    if (spapr_drc_type(drc) != SPAPR_DR_CONNECTOR_TYPE_PCI &&

        drc->allocation_state != SPAPR_DR_ALLOCATION_STATE_UNUSABLE) {

        trace_spapr_drc_awaiting_unusable(spapr_drc_index(drc));

        drc->awaiting_release = true;

        return;

    }



    if (drc->awaiting_allocation) {

        drc->awaiting_release = true;

        trace_spapr_drc_awaiting_allocation(spapr_drc_index(drc));

        return;

    }



    drc->dr_indicator = SPAPR_DR_INDICATOR_INACTIVE;



    

    switch (spapr_drc_type(drc)) {

    case SPAPR_DR_CONNECTOR_TYPE_CPU:

        spapr_core_release(drc->dev);

        break;

    case SPAPR_DR_CONNECTOR_TYPE_PCI:

        spapr_phb_remove_pci_device_cb(drc->dev);

        break;

    case SPAPR_DR_CONNECTOR_TYPE_LMB:

        spapr_lmb_release(drc->dev);

        break;

    case SPAPR_DR_CONNECTOR_TYPE_PHB:

    case SPAPR_DR_CONNECTOR_TYPE_VIO:

    default:

        g_assert(false);

    }



    drc->awaiting_release = false;

    g_free(drc->fdt);

    drc->fdt = NULL;

    drc->fdt_start_offset = 0;

    object_property_del(OBJECT(drc), "device", NULL);

    drc->dev = NULL;

}