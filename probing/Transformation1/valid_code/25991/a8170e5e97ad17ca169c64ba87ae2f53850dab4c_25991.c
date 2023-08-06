static void omap_sti_write(void *opaque, target_phys_addr_t addr,

                           uint64_t value, unsigned size)

{

    struct omap_sti_s *s = (struct omap_sti_s *) opaque;



    if (size != 4) {

        return omap_badwidth_write32(opaque, addr, value);

    }



    switch (addr) {

    case 0x00:	

    case 0x14:	

        OMAP_RO_REG(addr);

        return;



    case 0x10:	

        if (value & (1 << 1))				

            omap_sti_reset(s);

        s->sysconfig = value & 0xfe;

        break;



    case 0x18:	

        s->irqst &= ~value;

        omap_sti_interrupt_update(s);

        break;



    case 0x1c:	

        s->irqen = value & 0xffff;

        omap_sti_interrupt_update(s);

        break;



    case 0x2c:	

        s->clkcontrol = value & 0xff;

        break;



    case 0x30:	

        s->serial_config = value & 0xff;

        break;



    case 0x24:	

    case 0x28:	

        

        return;



    default:

        OMAP_BAD_REG(addr);

        return;

    }

}