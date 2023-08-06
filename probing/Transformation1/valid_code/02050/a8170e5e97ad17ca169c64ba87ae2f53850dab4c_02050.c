static void omap_pin_cfg_write(void *opaque, target_phys_addr_t addr,

                               uint64_t value, unsigned size)

{

    struct omap_mpu_state_s *s = (struct omap_mpu_state_s *) opaque;

    uint32_t diff;



    if (size != 4) {

        return omap_badwidth_write32(opaque, addr, value);

    }



    switch (addr) {

    case 0x00:	

        diff = s->func_mux_ctrl[addr >> 2] ^ value;

        s->func_mux_ctrl[addr >> 2] = value;

        omap_pin_funcmux0_update(s, diff, value);

        return;



    case 0x04:	

        diff = s->func_mux_ctrl[addr >> 2] ^ value;

        s->func_mux_ctrl[addr >> 2] = value;

        omap_pin_funcmux1_update(s, diff, value);

        return;



    case 0x08:	

        s->func_mux_ctrl[addr >> 2] = value;

        return;



    case 0x0c:	

        s->comp_mode_ctrl[0] = value;

        s->compat1509 = (value != 0x0000eaef);

        omap_pin_funcmux0_update(s, ~0, s->func_mux_ctrl[0]);

        omap_pin_funcmux1_update(s, ~0, s->func_mux_ctrl[1]);

        return;



    case 0x10:	

    case 0x14:	

    case 0x18:	

    case 0x1c:	

    case 0x20:	

    case 0x24:	

    case 0x28:	

    case 0x2c:	

    case 0x30:	

    case 0x34:	

    case 0x38:	

        s->func_mux_ctrl[(addr >> 2) - 1] = value;

        return;



    case 0x40:	

    case 0x44:	

    case 0x48:	

    case 0x4c:	

        s->pull_dwn_ctrl[(addr & 0xf) >> 2] = value;

        return;



    case 0x50:	

        s->gate_inh_ctrl[0] = value;

        return;



    case 0x60:	

        s->voltage_ctrl[0] = value;

        return;



    case 0x70:	

        s->test_dbg_ctrl[0] = value;

        return;



    case 0x80:	

        diff = s->mod_conf_ctrl[0] ^ value;

        s->mod_conf_ctrl[0] = value;

        omap_pin_modconf1_update(s, diff, value);

        return;



    default:

        OMAP_BAD_REG(addr);

    }

}