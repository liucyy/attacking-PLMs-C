static void strongarm_gpio_write(void *opaque, hwaddr offset,

                                 uint64_t value, unsigned size)

{

    StrongARMGPIOInfo *s = opaque;



    switch (offset) {

    case GPDR:        

        s->dir = value;

        strongarm_gpio_handler_update(s);

        break;



    case GPSR:        

        s->olevel |= value;

        strongarm_gpio_handler_update(s);

        s->gpsr = value;

        break;



    case GPCR:        

        s->olevel &= ~value;

        strongarm_gpio_handler_update(s);

        break;



    case GRER:        

        s->rising = value;

        break;



    case GFER:        

        s->falling = value;

        break;



    case GAFR:        

        s->gafr = value;

        break;



    case GEDR:        

        s->status &= ~value;

        strongarm_gpio_irq_update(s);

        break;



    default:

        printf("%s: Bad offset 0x" TARGET_FMT_plx "\n", __func__, offset);

    }

}