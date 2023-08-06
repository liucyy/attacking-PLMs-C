static void omap_pwt_write(void *opaque, target_phys_addr_t addr,

                           uint64_t value, unsigned size)

{

    struct omap_pwt_s *s = (struct omap_pwt_s *) opaque;

    int offset = addr & OMAP_MPUI_REG_MASK;



    if (size != 1) {

        return omap_badwidth_write8(opaque, addr, value);

    }



    switch (offset) {

    case 0x00:	

        s->frc = value & 0x3f;

        break;

    case 0x04:	

        if ((value ^ s->vrc) & 1) {

            if (value & 1)

                printf("%s: %iHz buzz on\n", __FUNCTION__, (int)

                                

                                ((omap_clk_getrate(s->clk) >> 3) /

                                 

                                 ((s->gcr & 2) ? 1 : 154) /

                                 

                                 (2 << (value & 3)) *

                                 

                                 ((value & (1 << 2)) ? 101 : 107) *

                                 

                                 ((value & (1 << 3)) ?  49 : 55) *

                                 

                                 ((value & (1 << 4)) ?  50 : 63) *

                                 

                                 ((value & (1 << 5)) ?  80 : 127) /

                                 (107 * 55 * 63 * 127)));

            else

                printf("%s: silence!\n", __FUNCTION__);

        }

        s->vrc = value & 0x7f;

        break;

    case 0x08:	

        s->gcr = value & 3;

        break;

    default:

        OMAP_BAD_REG(addr);

        return;

    }

}