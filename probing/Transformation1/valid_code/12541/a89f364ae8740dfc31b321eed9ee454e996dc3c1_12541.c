static void omap_mcbsp_writeh(void *opaque, hwaddr addr,

                uint32_t value)

{

    struct omap_mcbsp_s *s = (struct omap_mcbsp_s *) opaque;

    int offset = addr & OMAP_MPUI_REG_MASK;



    switch (offset) {

    case 0x00:	

    case 0x02:	

        OMAP_RO_REG(addr);

        return;



    case 0x04:	

        if (((s->xcr[0] >> 5) & 7) < 3)			

            return;

        

    case 0x06:	

        if (s->tx_req > 1) {

            s->tx_req -= 2;

            if (s->codec && s->codec->cts) {

                s->codec->out.fifo[s->codec->out.len ++] = (value >> 8) & 0xff;

                s->codec->out.fifo[s->codec->out.len ++] = (value >> 0) & 0xff;

            }

            if (s->tx_req < 2)

                omap_mcbsp_tx_done(s);

        } else

            printf("%s: Tx FIFO overrun\n", __FUNCTION__);

        return;



    case 0x08:	

        s->spcr[1] &= 0x0002;

        s->spcr[1] |= 0x03f9 & value;

        s->spcr[1] |= 0x0004 & (value << 2);		

        if (~value & 1)					

            s->spcr[1] &= ~6;

        omap_mcbsp_req_update(s);

        return;

    case 0x0a:	

        s->spcr[0] &= 0x0006;

        s->spcr[0] |= 0xf8f9 & value;

        if (value & (1 << 15))				

            printf("%s: Digital Loopback mode enable attempt\n", __FUNCTION__);

        if (~value & 1) {				

            s->spcr[0] &= ~6;

            s->rx_req = 0;

            omap_mcbsp_rx_done(s);

        }

        omap_mcbsp_req_update(s);

        return;



    case 0x0c:	

        s->rcr[1] = value & 0xffff;

        return;

    case 0x0e:	

        s->rcr[0] = value & 0x7fe0;

        return;

    case 0x10:	

        s->xcr[1] = value & 0xffff;

        return;

    case 0x12:	

        s->xcr[0] = value & 0x7fe0;

        return;

    case 0x14:	

        s->srgr[1] = value & 0xffff;

        omap_mcbsp_req_update(s);

        return;

    case 0x16:	

        s->srgr[0] = value & 0xffff;

        omap_mcbsp_req_update(s);

        return;

    case 0x18:	

        s->mcr[1] = value & 0x03e3;

        if (value & 3)					

            printf("%s: Tx channel selection mode enable attempt\n",

                            __FUNCTION__);

        return;

    case 0x1a:	

        s->mcr[0] = value & 0x03e1;

        if (value & 1)					

            printf("%s: Rx channel selection mode enable attempt\n",

                            __FUNCTION__);

        return;

    case 0x1c:	

        s->rcer[0] = value & 0xffff;

        return;

    case 0x1e:	

        s->rcer[1] = value & 0xffff;

        return;

    case 0x20:	

        s->xcer[0] = value & 0xffff;

        return;

    case 0x22:	

        s->xcer[1] = value & 0xffff;

        return;

    case 0x24:	

        s->pcr = value & 0x7faf;

        return;

    case 0x26:	

        s->rcer[2] = value & 0xffff;

        return;

    case 0x28:	

        s->rcer[3] = value & 0xffff;

        return;

    case 0x2a:	

        s->xcer[2] = value & 0xffff;

        return;

    case 0x2c:	

        s->xcer[3] = value & 0xffff;

        return;

    case 0x2e:	

        s->rcer[4] = value & 0xffff;

        return;

    case 0x30:	

        s->rcer[5] = value & 0xffff;

        return;

    case 0x32:	

        s->xcer[4] = value & 0xffff;

        return;

    case 0x34:	

        s->xcer[5] = value & 0xffff;

        return;

    case 0x36:	

        s->rcer[6] = value & 0xffff;

        return;

    case 0x38:	

        s->rcer[7] = value & 0xffff;

        return;

    case 0x3a:	

        s->xcer[6] = value & 0xffff;

        return;

    case 0x3c:	

        s->xcer[7] = value & 0xffff;

        return;

    }



    OMAP_BAD_REG(addr);

}