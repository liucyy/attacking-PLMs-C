VIOsPAPRDevice *vty_lookup(sPAPRMachineState *spapr, target_ulong reg)
{
    VIOsPAPRDevice *sdev;
    sdev = spapr_vio_find_by_reg(spapr->vio_bus, reg);
    if (!sdev && reg == 0) {
        
        return spapr_vty_get_default(spapr->vio_bus);
    return sdev;