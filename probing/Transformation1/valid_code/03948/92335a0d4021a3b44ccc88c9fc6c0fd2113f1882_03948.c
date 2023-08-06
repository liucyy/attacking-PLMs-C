static uint64_t strongarm_gpio_read(void *opaque, hwaddr offset,

                                    unsigned size)

{

    StrongARMGPIOInfo *s = opaque;



    switch (offset) {

    case GPDR:        

        return s->dir;



    case GPSR:        

        DPRINTF("%s: Read from a write-only register 0x" TARGET_FMT_plx "\n",

                        __func__, offset);

        return s->gpsr;    



    case GPCR:        

        DPRINTF("%s: Read from a write-only register 0x" TARGET_FMT_plx "\n",

                        __func__, offset);

        return 31337;        



    case GRER:        

        return s->rising;



    case GFER:        

        return s->falling;



    case GAFR:        

        return s->gafr;



    case GPLR:        

        return (s->olevel & s->dir) |

               (s->ilevel & ~s->dir);



    case GEDR:        

        return s->status;



    default:

        printf("%s: Bad offset 0x" TARGET_FMT_plx "\n", __func__, offset);

    }



    return 0;

}