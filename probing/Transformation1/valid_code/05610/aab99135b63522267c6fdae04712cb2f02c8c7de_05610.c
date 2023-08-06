static void detach(sPAPRDRConnector *drc, DeviceState *d,
                   spapr_drc_detach_cb *detach_cb,
                   void *detach_cb_opaque, Error **errp)
{
    DPRINTFN("drc: %x, detach", get_index(drc));
    drc->detach_cb = detach_cb;
    drc->detach_cb_opaque = detach_cb_opaque;
    
    if (!drc->signalled && !drc->configured) {
        
        drc->isolation_state = SPAPR_DR_ISOLATION_STATE_ISOLATED;
    if (drc->isolation_state != SPAPR_DR_ISOLATION_STATE_ISOLATED) {
        DPRINTFN("awaiting transition to isolated state before removal");
    if (drc->type != SPAPR_DR_CONNECTOR_TYPE_PCI &&
        drc->allocation_state != SPAPR_DR_ALLOCATION_STATE_UNUSABLE) {
        DPRINTFN("awaiting transition to unusable state before removal");
    drc->indicator_state = SPAPR_DR_INDICATOR_STATE_INACTIVE;
    if (drc->detach_cb) {
        drc->detach_cb(drc->dev, drc->detach_cb_opaque);
    drc->awaiting_release = false;
    g_free(drc->fdt);
    drc->fdt = NULL;
    drc->fdt_start_offset = 0;
    object_property_del(OBJECT(drc), "device", NULL);
    drc->dev = NULL;
    drc->detach_cb = NULL;
    drc->detach_cb_opaque = NULL;