static void omap_eac_write(void *opaque, target_phys_addr_t addr,

                           uint64_t value, unsigned size)

{

    struct omap_eac_s *s = (struct omap_eac_s *) opaque;



    if (size != 2) {

        return omap_badwidth_write16(opaque, addr, value);

    }



    switch (addr) {

    case 0x098:	

    case 0x09c:	

    case 0x0a0:	

    case 0x0a4:	

    case 0x0a8:	

    case 0x0ac:	

    case 0x0b0:	

    case 0x0b8:	

    case 0x0d0:	

    case 0x0d8:	

    case 0x0e4:	

    case 0x0ec:	

    case 0x100:	

    case 0x108:	

        OMAP_RO_REG(addr);

        return;



    case 0x000:	

        s->config[0] = value & 0xff;

        omap_eac_format_update(s);

        break;

    case 0x004:	

        s->config[1] = value & 0xff;

        omap_eac_format_update(s);

        break;

    case 0x008:	

        s->config[2] = value & 0xff;

        omap_eac_format_update(s);

        break;

    case 0x00c:	

        s->config[3] = value & 0xff;

        omap_eac_format_update(s);

        break;



    case 0x010:	

        

        s->control = value & 0x5f;

        omap_eac_interrupt_update(s);

        break;



    case 0x014:	

        s->address = value & 0xff;

        break;

    case 0x018:	

        s->data &= 0xff00;

        s->data |= value & 0xff;

        break;

    case 0x01c:	

        s->data &= 0x00ff;

        s->data |= value << 8;

        break;

    case 0x020:	

        s->vtol = value & 0xf8;

        break;

    case 0x024:	

        s->vtsl = value & 0x9f;

        break;

    case 0x040:	

        s->modem.control = value & 0x8f;

        break;

    case 0x044:	

        s->modem.config = value & 0x7fff;

        break;

    case 0x060:	

        s->bt.control = value & 0x8f;

        break;

    case 0x064:	

        s->bt.config = value & 0x7fff;

        break;

    case 0x080:	

        s->mixer = value & 0x0fff;

        break;

    case 0x084:	

        s->gain[0] = value & 0xffff;

        break;

    case 0x088:	

        s->gain[1] = value & 0xff7f;

        break;

    case 0x08c:	

        s->gain[2] = value & 0xff7f;

        break;

    case 0x090:	

        s->gain[3] = value & 0xff7f;

        break;

    case 0x094:	

        s->att = value & 0xff;

        break;



    case 0x0b4:	

        s->codec.txbuf[s->codec.txlen ++] = value;

        if (unlikely(s->codec.txlen == EAC_BUF_LEN ||

                                s->codec.txlen == s->codec.txavail)) {

            if (s->codec.txavail)

                omap_eac_out_empty(s);

            

            s->codec.txlen = 0;

        }

        break;



    case 0x0bc:	

        s->codec.config[0] = value & 0x07ff;

        omap_eac_format_update(s);

        break;

    case 0x0c0:	

        s->codec.config[1] = value & 0x780f;

        omap_eac_format_update(s);

        break;

    case 0x0c4:	

        s->codec.config[2] = value & 0x003f;

        omap_eac_format_update(s);

        break;

    case 0x0c8:	

        s->codec.config[3] = value & 0xffff;

        omap_eac_format_update(s);

        break;

    case 0x0cc:	

    case 0x0d4:	

    case 0x0e0:	

    case 0x0e8:	

    case 0x0f0:	

        break;



    case 0x104:	

        if (value & (1 << 1))				

            omap_eac_reset(s);

        s->sysconfig = value & 0x31d;

        break;



    default:

        OMAP_BAD_REG(addr);

        return;

    }

}