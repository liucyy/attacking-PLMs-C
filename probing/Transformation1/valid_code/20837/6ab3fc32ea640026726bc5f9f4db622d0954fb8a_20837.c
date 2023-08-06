static void imx_serial_write(void *opaque, hwaddr offset,

                             uint64_t value, unsigned size)

{

    IMXSerialState *s = (IMXSerialState *)opaque;

    unsigned char ch;



    DPRINTF("write(offset=0x%" HWADDR_PRIx ", value = 0x%x) to %s\n",

            offset, (unsigned int)value, s->chr ? s->chr->label : "NODEV");



    switch (offset >> 2) {

    case 0x10: 

        ch = value;

        if (s->ucr2 & UCR2_TXEN) {

            if (s->chr) {

                qemu_chr_fe_write(s->chr, &ch, 1);

            }

            s->usr1 &= ~USR1_TRDY;

            imx_update(s);

            s->usr1 |= USR1_TRDY;

            imx_update(s);

        }

        break;



    case 0x20: 

        s->ucr1 = value & 0xffff;



        DPRINTF("write(ucr1=%x)\n", (unsigned int)value);



        imx_update(s);

        break;



    case 0x21: 

        

        if (!(value & UCR2_SRST)) {

            imx_serial_reset(s);

            imx_update(s);

            value |= UCR2_SRST;

        }

        if (value & UCR2_RXEN) {

            if (!(s->ucr2 & UCR2_RXEN)) {

                if (s->chr) {

                    qemu_chr_accept_input(s->chr);

                }

            }

        }

        s->ucr2 = value & 0xffff;

        break;



    case 0x25: 

        value &= USR1_AWAKE | USR1_AIRINT | USR1_DTRD | USR1_AGTIM |

                 USR1_FRAMERR | USR1_ESCF | USR1_RTSD | USR1_PARTYER;

        s->usr1 &= ~value;

        break;



    case 0x26: 

        

        value &= USR2_ADET | USR2_DTRF | USR2_IDLE | USR2_ACST |

                 USR2_RIDELT | USR2_IRINT | USR2_WAKE |

                 USR2_DCDDELT | USR2_RTSF | USR2_BRCD | USR2_ORE;

        s->usr2 &= ~value;

        break;



    

    case 0x29: 

        s->ubrc = value & 0xffff;

        break;



    case 0x2a: 

        s->ubmr = value & 0xffff;

        break;



    case 0x2c: 

        s->onems = value & 0xffff;

        break;



    case 0x24: 

        s->ufcr = value & 0xffff;

        break;



    case 0x22: 

        s->ucr3 = value & 0xffff;

        break;



    case 0x2d: 

    case 0x23: 

        qemu_log_mask(LOG_UNIMP, "[%s]%s: Unimplemented reg 0x%"

                      HWADDR_PRIx "\n", TYPE_IMX_SERIAL, __func__, offset);

        

        break;



    default:

        qemu_log_mask(LOG_GUEST_ERROR, "[%s]%s: Bad register at offset 0x%"

                      HWADDR_PRIx "\n", TYPE_IMX_SERIAL, __func__, offset);

    }

}