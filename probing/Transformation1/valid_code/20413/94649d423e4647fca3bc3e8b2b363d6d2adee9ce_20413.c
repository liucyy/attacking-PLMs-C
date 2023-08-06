sPAPRDRConnector *spapr_dr_connector_new(Object *owner,

                                         sPAPRDRConnectorType type,

                                         uint32_t id)

{

    sPAPRDRConnector *drc =

        SPAPR_DR_CONNECTOR(object_new(TYPE_SPAPR_DR_CONNECTOR));



    g_assert(type);



    drc->type = type;

    drc->id = id;

    drc->owner = owner;

    object_property_add_child(owner, "dr-connector[*]", OBJECT(drc), NULL);

    object_property_set_bool(OBJECT(drc), true, "realized", NULL);



    

    switch (drc->type) {

    case SPAPR_DR_CONNECTOR_TYPE_CPU:

        drc->name = g_strdup_printf("CPU %d", id);

        break;

    case SPAPR_DR_CONNECTOR_TYPE_PHB:

        drc->name = g_strdup_printf("PHB %d", id);

        break;

    case SPAPR_DR_CONNECTOR_TYPE_VIO:

    case SPAPR_DR_CONNECTOR_TYPE_PCI:

        drc->name = g_strdup_printf("C%d", id);

        break;

    case SPAPR_DR_CONNECTOR_TYPE_LMB:

        drc->name = g_strdup_printf("LMB %d", id);

        break;

    default:

        g_assert(false);

    }



    

    if (drc->type == SPAPR_DR_CONNECTOR_TYPE_PCI) {

        drc->allocation_state = SPAPR_DR_ALLOCATION_STATE_USABLE;

    }



    return drc;

}