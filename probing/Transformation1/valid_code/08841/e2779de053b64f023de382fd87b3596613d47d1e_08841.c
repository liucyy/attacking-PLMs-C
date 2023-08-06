static int xen_pt_msgaddr32_reg_write(XenPCIPassthroughState *s,

                                      XenPTReg *cfg_entry, uint32_t *val,

                                      uint32_t dev_value, uint32_t valid_mask)

{

    XenPTRegInfo *reg = cfg_entry->reg;

    uint32_t writable_mask = 0;

    uint32_t old_addr = cfg_entry->data;



    

    writable_mask = reg->emu_mask & ~reg->ro_mask & valid_mask;

    cfg_entry->data = XEN_PT_MERGE_VALUE(*val, cfg_entry->data, writable_mask);

    s->msi->addr_lo = cfg_entry->data;



    

    *val = XEN_PT_MERGE_VALUE(*val, dev_value, 0);



    

    if (cfg_entry->data != old_addr) {

        if (s->msi->mapped) {

            xen_pt_msi_update(s);

        }

    }



    return 0;

}