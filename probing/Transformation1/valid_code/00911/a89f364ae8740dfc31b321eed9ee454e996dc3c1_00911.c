static void pxa2xx_gpio_write(void *opaque, hwaddr offset,

                              uint64_t value, unsigned size)

{

    PXA2xxGPIOInfo *s = (PXA2xxGPIOInfo *) opaque;

    int bank;

    if (offset >= 0x200)

        return;



    bank = pxa2xx_gpio_regs[offset].bank;

    switch (pxa2xx_gpio_regs[offset].reg) {

    case GPDR:		

        s->dir[bank] = value;

        pxa2xx_gpio_handler_update(s);

        break;



    case GPSR:		

        s->olevel[bank] |= value;

        pxa2xx_gpio_handler_update(s);

        break;



    case GPCR:		

        s->olevel[bank] &= ~value;

        pxa2xx_gpio_handler_update(s);

        break;



    case GRER:		

        s->rising[bank] = value;

        break;



    case GFER:		

        s->falling[bank] = value;

        break;



    case GAFR_L:	

        s->gafr[bank * 2] = value;

        break;



    case GAFR_U:	

        s->gafr[bank * 2 + 1] = value;

        break;



    case GEDR:		

        s->status[bank] &= ~value;

        pxa2xx_gpio_irq_update(s);

        break;



    default:

        hw_error("%s: Bad offset " REG_FMT "\n", __FUNCTION__, offset);

    }

}