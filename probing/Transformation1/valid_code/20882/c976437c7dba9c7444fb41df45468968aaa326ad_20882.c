static int xen_pt_msgctrl_reg_write(XenPCIPassthroughState *s,

                                    XenPTReg *cfg_entry, uint16_t *val,

                                    uint16_t dev_value, uint16_t valid_mask)

{

    XenPTRegInfo *reg = cfg_entry->reg;

    XenPTMSI *msi = s->msi;

    uint16_t writable_mask = 0;

    uint16_t throughable_mask = 0;

    uint16_t raw_val;



    

    if (*val & PCI_MSI_FLAGS_QSIZE) {

        XEN_PT_WARN(&s->dev, "Tries to set more than 1 vector ctrl %x\n", *val);

    }



    

    writable_mask = reg->emu_mask & ~reg->ro_mask & valid_mask;

    cfg_entry->data = XEN_PT_MERGE_VALUE(*val, cfg_entry->data, writable_mask);

    msi->flags |= cfg_entry->data & ~PCI_MSI_FLAGS_ENABLE;



    

    raw_val = *val;

    throughable_mask = ~reg->emu_mask & valid_mask;

    *val = XEN_PT_MERGE_VALUE(*val, dev_value, throughable_mask);



    

    if (raw_val & PCI_MSI_FLAGS_ENABLE) {

        

        if (!msi->initialized) {

            

            XEN_PT_LOG(&s->dev, "setup MSI\n");

            if (xen_pt_msi_setup(s)) {

                

                *val &= ~PCI_MSI_FLAGS_ENABLE;

                XEN_PT_WARN(&s->dev, "Can not map MSI.\n");

                return 0;

            }

            if (xen_pt_msi_update(s)) {

                *val &= ~PCI_MSI_FLAGS_ENABLE;

                XEN_PT_WARN(&s->dev, "Can not bind MSI\n");

                return 0;

            }

            msi->initialized = true;

            msi->mapped = true;

        }

        msi->flags |= PCI_MSI_FLAGS_ENABLE;

    } else {

        msi->flags &= ~PCI_MSI_FLAGS_ENABLE;

    }



    

    *val &= ~PCI_MSI_FLAGS_ENABLE;

    *val |= raw_val & PCI_MSI_FLAGS_ENABLE;



    return 0;

}