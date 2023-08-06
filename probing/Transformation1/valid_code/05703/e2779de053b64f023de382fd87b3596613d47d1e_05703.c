static int xen_pt_msgdata_reg_write(XenPCIPassthroughState *s,

                                    XenPTReg *cfg_entry, uint16_t *val,

                                    uint16_t dev_value, uint16_t valid_mask)

{

    XenPTRegInfo *reg = cfg_entry->reg;

    XenPTMSI *msi = s->msi;

    uint16_t writable_mask = 0;

    uint16_t old_data = cfg_entry->data;

    uint32_t offset = reg->offset;



    

    if (!xen_pt_msi_check_type(offset, msi->flags, DATA)) {

        

        XEN_PT_ERR(&s->dev, "the offset does not match the 32/64 bit type!\n");

        return -1;

    }



    

    writable_mask = reg->emu_mask & ~reg->ro_mask & valid_mask;

    cfg_entry->data = XEN_PT_MERGE_VALUE(*val, cfg_entry->data, writable_mask);

    

    msi->data = cfg_entry->data;



    

    *val = XEN_PT_MERGE_VALUE(*val, dev_value, 0);



    

    if (cfg_entry->data != old_data) {

        if (msi->mapped) {

            xen_pt_msi_update(s);

        }

    }



    return 0;

}