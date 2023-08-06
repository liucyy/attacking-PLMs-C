static uint64_t omap_ulpd_pm_read(void *opaque, target_phys_addr_t addr,

                                  unsigned size)

{

    struct omap_mpu_state_s *s = (struct omap_mpu_state_s *) opaque;

    uint16_t ret;



    if (size != 2) {

        return omap_badwidth_read16(opaque, addr);

    }



    switch (addr) {

    case 0x14:	

        ret = s->ulpd_pm_regs[addr >> 2];

        s->ulpd_pm_regs[addr >> 2] = 0;

        qemu_irq_lower(s->irq[1][OMAP_INT_GAUGE_32K]);

        return ret;



    case 0x18:	

    case 0x1c:	

    case 0x20:	

    case 0x28:	

    case 0x2c:	

        OMAP_BAD_REG(addr);

    case 0x00:	

    case 0x04:	

    case 0x08:	

    case 0x0c:	

    case 0x10:	

    case 0x24:	

    case 0x30:	

    case 0x34:	

    case 0x38:	

    case 0x3c:	

    case 0x40:	

        

    case 0x48:	

    case 0x4c:	

    case 0x50:	

        return s->ulpd_pm_regs[addr >> 2];

    }



    OMAP_BAD_REG(addr);

    return 0;

}