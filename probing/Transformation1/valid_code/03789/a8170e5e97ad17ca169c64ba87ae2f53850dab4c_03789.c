static uint64_t omap_sti_read(void *opaque, target_phys_addr_t addr,

                              unsigned size)

{

    struct omap_sti_s *s = (struct omap_sti_s *) opaque;



    if (size != 4) {

        return omap_badwidth_read32(opaque, addr);

    }



    switch (addr) {

    case 0x00:	

        return 0x10;



    case 0x10:	

        return s->sysconfig;



    case 0x14:	

        return 0x00;



    case 0x18:	

        return s->irqst;



    case 0x1c:	

        return s->irqen;



    case 0x24:	

    case 0x28:	

        

        return 0;



    case 0x2c:	

        return s->clkcontrol;



    case 0x30:	

        return s->serial_config;

    }



    OMAP_BAD_REG(addr);

    return 0;

}