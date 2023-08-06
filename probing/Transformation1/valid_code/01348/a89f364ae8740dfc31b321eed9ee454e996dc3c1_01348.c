static uint64_t pxa2xx_pic_mem_read(void *opaque, hwaddr offset,

                                    unsigned size)

{

    PXA2xxPICState *s = (PXA2xxPICState *) opaque;



    switch (offset) {

    case ICIP:	

        return s->int_pending[0] & ~s->is_fiq[0] & s->int_enabled[0];

    case ICIP2:	

        return s->int_pending[1] & ~s->is_fiq[1] & s->int_enabled[1];

    case ICMR:	

        return s->int_enabled[0];

    case ICMR2:	

        return s->int_enabled[1];

    case ICLR:	

        return s->is_fiq[0];

    case ICLR2:	

        return s->is_fiq[1];

    case ICCR:	

        return (s->int_idle == 0);

    case ICFP:	

        return s->int_pending[0] & s->is_fiq[0] & s->int_enabled[0];

    case ICFP2:	

        return s->int_pending[1] & s->is_fiq[1] & s->int_enabled[1];

    case ICPR:	

        return s->int_pending[0];

    case ICPR2:	

        return s->int_pending[1];

    case IPR0  ... IPR31:

        return s->priority[0  + ((offset - IPR0 ) >> 2)];

    case IPR32 ... IPR39:

        return s->priority[32 + ((offset - IPR32) >> 2)];

    case ICHP:	

        return pxa2xx_pic_highest(s);

    default:

        printf("%s: Bad register offset " REG_FMT "\n", __FUNCTION__, offset);

        return 0;

    }

}