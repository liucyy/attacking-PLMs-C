static uint64_t pxa2xx_gpio_read(void *opaque, hwaddr offset,

                                 unsigned size)

{

    PXA2xxGPIOInfo *s = (PXA2xxGPIOInfo *) opaque;

    uint32_t ret;

    int bank;

    if (offset >= 0x200)

        return 0;



    bank = pxa2xx_gpio_regs[offset].bank;

    switch (pxa2xx_gpio_regs[offset].reg) {

    case GPDR:		

        return s->dir[bank];



    case GPSR:		

        printf("%s: Read from a write-only register " REG_FMT "\n",

                        __FUNCTION__, offset);

        return s->gpsr[bank];	



    case GPCR:		

        printf("%s: Read from a write-only register " REG_FMT "\n",

                        __FUNCTION__, offset);

        return 31337;		



    case GRER:		

        return s->rising[bank];



    case GFER:		

        return s->falling[bank];



    case GAFR_L:	

        return s->gafr[bank * 2];



    case GAFR_U:	

        return s->gafr[bank * 2 + 1];



    case GPLR:		

        ret = (s->olevel[bank] & s->dir[bank]) |

                (s->ilevel[bank] & ~s->dir[bank]);

        qemu_irq_raise(s->read_notify);

        return ret;



    case GEDR:		

        return s->status[bank];



    default:

        hw_error("%s: Bad offset " REG_FMT "\n", __FUNCTION__, offset);

    }



    return 0;

}