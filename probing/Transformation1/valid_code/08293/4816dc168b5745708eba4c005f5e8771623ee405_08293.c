static void imx_fec_write(void *opaque, hwaddr addr,

                          uint64_t value, unsigned size)

{

    IMXFECState *s = IMX_FEC(opaque);



    FEC_PRINTF("writing 0x%08x @ 0x%" HWADDR_PRIx "\n", (int)value, addr);



    switch (addr & 0x3ff) {

    case 0x004: 

        s->eir &= ~value;

        break;

    case 0x008: 

        s->eimr = value;

        break;

    case 0x010: 

        if ((s->ecr & FEC_EN) && !s->rx_enabled) {

            imx_fec_enable_rx(s);

        }

        break;

    case 0x014: 

        if (s->ecr & FEC_EN) {

            imx_fec_do_tx(s);

        }

        break;

    case 0x024: 

        s->ecr = value;

        if (value & FEC_RESET) {

            imx_fec_reset(DEVICE(s));

        }

        if ((s->ecr & FEC_EN) == 0) {

            s->rx_enabled = 0;

        }

        break;

    case 0x040: 

        

        s->mmfr = value;

        if (extract32(value, 28, 1)) {

            do_phy_write(s, extract32(value, 18, 9), extract32(value, 0, 16));

        } else {

            s->mmfr = do_phy_read(s, extract32(value, 18, 9));

        }

        

        s->eir |= FEC_INT_MII;

        break;

    case 0x044: 

        s->mscr = value & 0xfe;

        break;

    case 0x064: 

        

        s->mibc = (value & 0x80000000) ? 0xc0000000 : 0;

        break;

    case 0x084: 

        s->rcr = value & 0x07ff003f;

        

        break;

    case 0x0c4: 

        

        s->tcr = value;

        if (value & 1) {

            s->eir |= FEC_INT_GRA;

        }

        break;

    case 0x0e4: 

        s->conf.macaddr.a[0] = value >> 24;

        s->conf.macaddr.a[1] = value >> 16;

        s->conf.macaddr.a[2] = value >> 8;

        s->conf.macaddr.a[3] = value;

        break;

    case 0x0e8: 

        s->conf.macaddr.a[4] = value >> 24;

        s->conf.macaddr.a[5] = value >> 16;

        break;

    case 0x0ec: 

        break;

    case 0x118: 

    case 0x11c: 

    case 0x120: 

    case 0x124: 

        

        break;

    case 0x144: 

        s->tfwr = value & 3;

        break;

    case 0x14c: 

        

        break;

    case 0x150: 

        s->frsr = (value & 0x3fc) | 0x400;

        break;

    case 0x180: 

        s->erdsr = value & ~3;

        s->rx_descriptor = s->erdsr;

        break;

    case 0x184: 

        s->etdsr = value & ~3;

        s->tx_descriptor = s->etdsr;

        break;

    case 0x188: 

        s->emrbr = value & 0x7f0;

        break;

    case 0x300: 

        s->miigsk_cfgr = value & 0x53;

        break;

    case 0x308: 

        s->miigsk_enr = (value & 0x2) ? 0x6 : 0;

        break;

    default:

        qemu_log_mask(LOG_GUEST_ERROR, "[%s]%s: Bad address at offset 0x%"

                      HWADDR_PRIx "\n", TYPE_IMX_FEC, __func__, addr);

        break;

    }



    imx_fec_update(s);

}