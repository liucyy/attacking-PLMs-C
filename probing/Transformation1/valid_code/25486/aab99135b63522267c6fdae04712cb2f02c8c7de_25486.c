static void attach(sPAPRDRConnector *drc, DeviceState *d, void *fdt,
                   int fdt_start_offset, bool coldplug, Error **errp)
{
    DPRINTFN("drc: %x, attach", get_index(drc));
    if (drc->isolation_state != SPAPR_DR_ISOLATION_STATE_ISOLATED) {
        error_setg(errp, "an attached device is still awaiting release");
        return;
    if (drc->type == SPAPR_DR_CONNECTOR_TYPE_PCI) {
        g_assert(drc->allocation_state == SPAPR_DR_ALLOCATION_STATE_USABLE);
    g_assert(fdt || coldplug);
    
    if (drc->type == SPAPR_DR_CONNECTOR_TYPE_PCI) {
        drc->isolation_state = SPAPR_DR_ISOLATION_STATE_UNISOLATED;
    drc->indicator_state = SPAPR_DR_INDICATOR_STATE_ACTIVE;
    drc->dev = d;
    drc->fdt = fdt;
    drc->fdt_start_offset = fdt_start_offset;
    drc->configured = coldplug;
    
    drc->signalled = (drc->type != SPAPR_DR_CONNECTOR_TYPE_PCI)
                     ? true : coldplug;
    object_property_add_link(OBJECT(drc), "device",
                             object_get_typename(OBJECT(drc->dev)),
                             (Object **)(&drc->dev),
                             NULL, 0, NULL);