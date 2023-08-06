static uint64_t omap_pin_cfg_read(void *opaque, target_phys_addr_t addr,

                                  unsigned size)

{

    struct omap_mpu_state_s *s = (struct omap_mpu_state_s *) opaque;



    if (size != 4) {

        return omap_badwidth_read32(opaque, addr);

    }



    switch (addr) {

    case 0x00:	

    case 0x04:	

    case 0x08:	

        return s->func_mux_ctrl[addr >> 2];



    case 0x0c:	

        return s->comp_mode_ctrl[0];



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

        return s->func_mux_ctrl[(addr >> 2) - 1];



    case 0x40:	

    case 0x44:	

    case 0x48:	

    case 0x4c:	

        return s->pull_dwn_ctrl[(addr & 0xf) >> 2];



    case 0x50:	

        return s->gate_inh_ctrl[0];



    case 0x60:	

        return s->voltage_ctrl[0];



    case 0x70:	

        return s->test_dbg_ctrl[0];



    case 0x80:	

        return s->mod_conf_ctrl[0];

    }



    OMAP_BAD_REG(addr);

    return 0;

}