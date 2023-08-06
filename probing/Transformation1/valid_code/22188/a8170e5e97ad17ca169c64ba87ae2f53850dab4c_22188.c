static void omap_mpui_write(void *opaque, target_phys_addr_t addr,

                            uint64_t value, unsigned size)

{

    struct omap_mpu_state_s *s = (struct omap_mpu_state_s *) opaque;



    if (size != 4) {

        return omap_badwidth_write32(opaque, addr, value);

    }



    switch (addr) {

    case 0x00:	

        s->mpui_ctrl = value & 0x007fffff;

        break;



    case 0x04:	

    case 0x08:	

    case 0x0c:	

    case 0x10:	

    

    case 0x14:	

        OMAP_RO_REG(addr);

    case 0x18:	

    case 0x1c:	

        break;



    default:

        OMAP_BAD_REG(addr);

    }

}