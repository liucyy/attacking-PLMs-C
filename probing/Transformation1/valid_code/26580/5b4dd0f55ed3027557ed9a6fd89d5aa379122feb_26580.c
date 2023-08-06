static int xen_pt_config_reg_init(XenPCIPassthroughState *s,

                                  XenPTRegGroup *reg_grp, XenPTRegInfo *reg)

{

    XenPTReg *reg_entry;

    uint32_t data = 0;

    int rc = 0;



    reg_entry = g_new0(XenPTReg, 1);

    reg_entry->reg = reg;



    if (reg->init) {

        uint32_t host_mask, size_mask;

        unsigned int offset;

        uint32_t val;



        

        rc = reg->init(s, reg_entry->reg,

                       reg_grp->base_offset + reg->offset, &data);

        if (rc < 0) {

            g_free(reg_entry);

            return rc;

        }

        if (data == XEN_PT_INVALID_REG) {

            

            g_free(reg_entry);

            return 0;

        }

        

        offset = reg_grp->base_offset + reg->offset;

        size_mask = 0xFFFFFFFF >> ((4 - reg->size) << 3);



        switch (reg->size) {

        case 1: rc = xen_host_pci_get_byte(&s->real_device, offset, (uint8_t *)&val);

                break;

        case 2: rc = xen_host_pci_get_word(&s->real_device, offset, (uint16_t *)&val);

                break;

        case 4: rc = xen_host_pci_get_long(&s->real_device, offset, &val);

                break;

        default: assert(1);

        }

        if (rc) {

            

            g_free(reg_entry);

            return rc;

        }

        

        host_mask = size_mask & ~reg->emu_mask;



        if ((data & host_mask) != (val & host_mask)) {

            uint32_t new_val;



            

            new_val = val & host_mask;

            

            new_val |= data & host_mask;

            

            new_val |= ((val | data)) & ~size_mask;

            XEN_PT_LOG(&s->dev,"Offset 0x%04x mismatch! Emulated=0x%04x, host=0x%04x, syncing to 0x%04x.\n",

                       offset, data, val, new_val);

            val = new_val;

        } else

            val = data;



        

        switch (reg->size) {

        case 1: pci_set_byte(s->dev.config + offset, (uint8_t)val);

                break;

        case 2: pci_set_word(s->dev.config + offset, (uint16_t)val);

                break;

        case 4: pci_set_long(s->dev.config + offset, val);

                break;

        default: assert(1);

        }

        

        reg_entry->data = val;



    }

    

    QLIST_INSERT_HEAD(&reg_grp->reg_tbl_list, reg_entry, entries);



    return 0;

}