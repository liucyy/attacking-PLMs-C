static void realize(DeviceState *d, Error **errp)

{

    sPAPRDRConnector *drc = SPAPR_DR_CONNECTOR(d);

    sPAPRDRConnectorClass *drck = SPAPR_DR_CONNECTOR_GET_CLASS(drc);

    Object *root_container;

    char link_name[256];

    gchar *child_name;

    Error *err = NULL;



    DPRINTFN("drc realize: %x", drck->get_index(drc));

    

    root_container = container_get(object_get_root(), DRC_CONTAINER_PATH);

    snprintf(link_name, sizeof(link_name), "%x", drck->get_index(drc));

    child_name = object_get_canonical_path_component(OBJECT(drc));

    DPRINTFN("drc child name: %s", child_name);

    object_property_add_alias(root_container, link_name,

                              drc->owner, child_name, &err);

    if (err) {

        error_report("%s", error_get_pretty(err));

        error_free(err);

        object_unref(OBJECT(drc));

    }


    DPRINTFN("drc realize complete");

}