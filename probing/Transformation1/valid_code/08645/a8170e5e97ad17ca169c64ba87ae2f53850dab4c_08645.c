static uint64_t omap_tipb_bridge_read(void *opaque, target_phys_addr_t addr,

                                      unsigned size)

{

    struct omap_tipb_bridge_s *s = (struct omap_tipb_bridge_s *) opaque;



    if (size < 2) {

        return omap_badwidth_read16(opaque, addr);

    }



    switch (addr) {

    case 0x00:	

        return s->control;

    case 0x04:	

        return s->alloc;

    case 0x08:	

        return s->buffer;

    case 0x0c:	

        return s->enh_control;

    case 0x10:	

    case 0x14:	

    case 0x18:	

        return 0xffff;

    case 0x1c:	

        return 0x00f8;

    }



    OMAP_BAD_REG(addr);

    return 0;

}