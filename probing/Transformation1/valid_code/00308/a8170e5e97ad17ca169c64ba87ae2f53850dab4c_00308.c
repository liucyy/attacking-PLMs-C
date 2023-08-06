static void omap_tipb_bridge_write(void *opaque, target_phys_addr_t addr,

                                   uint64_t value, unsigned size)

{

    struct omap_tipb_bridge_s *s = (struct omap_tipb_bridge_s *) opaque;



    if (size < 2) {

        return omap_badwidth_write16(opaque, addr, value);

    }



    switch (addr) {

    case 0x00:	

        s->control = value & 0xffff;

        break;



    case 0x04:	

        s->alloc = value & 0x003f;

        break;



    case 0x08:	

        s->buffer = value & 0x0003;

        break;



    case 0x0c:	

        s->width_intr = !(value & 2);

        s->enh_control = value & 0x000f;

        break;



    case 0x10:	

    case 0x14:	

    case 0x18:	

    case 0x1c:	

        OMAP_RO_REG(addr);

        break;



    default:

        OMAP_BAD_REG(addr);

    }

}