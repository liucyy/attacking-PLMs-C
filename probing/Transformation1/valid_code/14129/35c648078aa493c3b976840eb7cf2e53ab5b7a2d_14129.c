static uint64_t malta_fpga_read(void *opaque, hwaddr addr,

                                unsigned size)

{

    MaltaFPGAState *s = opaque;

    uint32_t val = 0;

    uint32_t saddr;



    saddr = (addr & 0xfffff);



    switch (saddr) {



    

    case 0x00200:

        val = 0x00000000;		

        break;



    

    case 0x00208:

#ifdef TARGET_WORDS_BIGENDIAN

        val = 0x00000012;

#else

        val = 0x00000010;

#endif

        break;



    

    case 0x00210:

        val = 0x00;

        break;



    

    case 0x00408:

        val = s->leds;

        break;



    

    case 0x00508:

        val = s->brk;

        break;



    



    

    case 0x00a00:

        val = s->gpout;

        break;



    



    

    case 0x00a08:

        

        if (s->i2csel)

            val = s->i2cout;

        else

            val = 0x00;

        break;



    

    case 0x00b00:

        val = ((s->i2cin & ~1) | eeprom24c0x_read());

        break;



    

    case 0x00b08:

        val = s->i2coe;

        break;



    

    case 0x00b10:

        val = s->i2cout;

        break;



    

    case 0x00b18:

        val = s->i2csel;

        break;



    default:

#if 0

        printf ("malta_fpga_read: Bad register offset 0x" TARGET_FMT_lx "\n",

                addr);

#endif

        break;

    }

    return val;

}