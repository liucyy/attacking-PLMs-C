static uint64_t omap_clkm_read(void *opaque, target_phys_addr_t addr,

                               unsigned size)

{

    struct omap_mpu_state_s *s = (struct omap_mpu_state_s *) opaque;



    if (size != 2) {

        return omap_badwidth_read16(opaque, addr);

    }



    switch (addr) {

    case 0x00:	

        return s->clkm.arm_ckctl;



    case 0x04:	

        return s->clkm.arm_idlect1;



    case 0x08:	

        return s->clkm.arm_idlect2;



    case 0x0c:	

        return s->clkm.arm_ewupct;



    case 0x10:	

        return s->clkm.arm_rstct1;



    case 0x14:	

        return s->clkm.arm_rstct2;



    case 0x18:	

        return (s->clkm.clocking_scheme << 11) | s->clkm.cold_start;



    case 0x1c:	

        return s->clkm.arm_ckout1;



    case 0x20:	

        break;

    }



    OMAP_BAD_REG(addr);

    return 0;

}