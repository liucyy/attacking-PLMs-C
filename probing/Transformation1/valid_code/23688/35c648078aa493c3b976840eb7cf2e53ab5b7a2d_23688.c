static void malta_fpga_write(void *opaque, hwaddr addr,

                             uint64_t val, unsigned size)

{

    MaltaFPGAState *s = opaque;

    uint32_t saddr;



    saddr = (addr & 0xfffff);



    switch (saddr) {



    

    case 0x00200:

        break;



    

    case 0x00210:

        break;



    

    case 0x00408:

        s->leds = val & 0xff;

        malta_fpga_update_display(s);

        break;



    

    case 0x00410:

        snprintf(s->display_text, 9, "%08X", (uint32_t)val);

        malta_fpga_update_display(s);

        break;



    

    case 0x00418:

    case 0x00420:

    case 0x00428:

    case 0x00430:

    case 0x00438:

    case 0x00440:

    case 0x00448:

    case 0x00450:

        s->display_text[(saddr - 0x00418) >> 3] = (char) val;

        malta_fpga_update_display(s);

        break;



    

    case 0x00500:

        if (val == 0x42)

            qemu_system_reset_request ();

        break;



    

    case 0x00508:

        s->brk = val & 0xff;

        break;



    



    

    case 0x00a00:

        s->gpout = val & 0xff;

        break;



    

    case 0x00b08:

        s->i2coe = val & 0x03;

        break;



    

    case 0x00b10:

        eeprom24c0x_write(val & 0x02, val & 0x01);

        s->i2cout = val;

        break;



    

    case 0x00b18:

        s->i2csel = val & 0x01;

        break;



    default:

#if 0

        printf ("malta_fpga_write: Bad register offset 0x" TARGET_FMT_lx "\n",

                addr);

#endif

        break;

    }

}