static uint64_t omap_eac_read(void *opaque, target_phys_addr_t addr,

                              unsigned size)

{

    struct omap_eac_s *s = (struct omap_eac_s *) opaque;

    uint32_t ret;



    if (size != 2) {

        return omap_badwidth_read16(opaque, addr);

    }



    switch (addr) {

    case 0x000:	

        return s->config[0];

    case 0x004:	

        return s->config[1];

    case 0x008:	

        return s->config[2];

    case 0x00c:	

        return s->config[3];



    case 0x010:	

        return s->control | ((s->codec.rxavail + s->codec.rxlen > 0) << 7) |

                ((s->codec.txlen < s->codec.txavail) << 5);



    case 0x014:	

        return s->address;

    case 0x018:	

        return s->data & 0xff;

    case 0x01c:	

        return s->data >> 8;

    case 0x020:	

        return s->vtol;

    case 0x024:	

        return s->vtsl | (3 << 5);	

    case 0x040:	

        return s->modem.control;

    case 0x044:	

        return s->modem.config;

    case 0x060:	

        return s->bt.control;

    case 0x064:	

        return s->bt.config;

    case 0x080:	

        return s->mixer;

    case 0x084:	

        return s->gain[0];

    case 0x088:	

        return s->gain[1];

    case 0x08c:	

        return s->gain[2];

    case 0x090:	

        return s->gain[3];

    case 0x094:	

        return s->att;

    case 0x098:	

        return s->max[0];

    case 0x09c:	

        return s->max[1];

    case 0x0a0:	

        return s->max[2];

    case 0x0a4:	

        return s->max[3];

    case 0x0a8:	

        return s->max[4];

    case 0x0ac:	

        return s->max[5];

    case 0x0b0:	

        return s->max[6];

    case 0x0b4:	

        

        return 0x0000;

    case 0x0b8:	

        if (likely(s->codec.rxlen > 1)) {

            ret = s->codec.rxbuf[s->codec.rxoff ++];

            s->codec.rxlen --;

            s->codec.rxoff &= EAC_BUF_LEN - 1;

            return ret;

        } else if (s->codec.rxlen) {

            ret = s->codec.rxbuf[s->codec.rxoff ++];

            s->codec.rxlen --;

            s->codec.rxoff &= EAC_BUF_LEN - 1;

            if (s->codec.rxavail)

                omap_eac_in_refill(s);

            omap_eac_in_dmarequest_update(s);

            return ret;

        }

        return 0x0000;

    case 0x0bc:	

        return s->codec.config[0];

    case 0x0c0:	

        return s->codec.config[1] | ((s->codec.config[1] & 2) << 14);

    case 0x0c4:	

        return s->codec.config[2];

    case 0x0c8:	

        return s->codec.config[3];

    case 0x0cc:	

    case 0x0d0:	

    case 0x0d8:	

    case 0x0e4:	

    case 0x0ec:	

        return 0x0000;



    case 0x100:	

        return 0x0010;



    case 0x104:	

        return s->sysconfig;



    case 0x108:	

        return 1 | 0xe;					

    }



    OMAP_BAD_REG(addr);

    return 0;

}