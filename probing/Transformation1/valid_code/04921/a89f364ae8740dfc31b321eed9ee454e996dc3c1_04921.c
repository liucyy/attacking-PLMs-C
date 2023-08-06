static uint64_t omap_mcbsp_read(void *opaque, hwaddr addr,

                                unsigned size)

{

    struct omap_mcbsp_s *s = (struct omap_mcbsp_s *) opaque;

    int offset = addr & OMAP_MPUI_REG_MASK;

    uint16_t ret;



    if (size != 2) {

        return omap_badwidth_read16(opaque, addr);

    }



    switch (offset) {

    case 0x00:	

        if (((s->rcr[0] >> 5) & 7) < 3)			

            return 0x0000;

        

    case 0x02:	

        if (s->rx_req < 2) {

            printf("%s: Rx FIFO underrun\n", __FUNCTION__);

            omap_mcbsp_rx_done(s);

        } else {

            s->tx_req -= 2;

            if (s->codec && s->codec->in.len >= 2) {

                ret = s->codec->in.fifo[s->codec->in.start ++] << 8;

                ret |= s->codec->in.fifo[s->codec->in.start ++];

                s->codec->in.len -= 2;

            } else

                ret = 0x0000;

            if (!s->tx_req)

                omap_mcbsp_rx_done(s);

            return ret;

        }

        return 0x0000;



    case 0x04:	

    case 0x06:	

        return 0x0000;



    case 0x08:	

        return s->spcr[1];

    case 0x0a:	

        return s->spcr[0];

    case 0x0c:	

        return s->rcr[1];

    case 0x0e:	

        return s->rcr[0];

    case 0x10:	

        return s->xcr[1];

    case 0x12:	

        return s->xcr[0];

    case 0x14:	

        return s->srgr[1];

    case 0x16:	

        return s->srgr[0];

    case 0x18:	

        return s->mcr[1];

    case 0x1a:	

        return s->mcr[0];

    case 0x1c:	

        return s->rcer[0];

    case 0x1e:	

        return s->rcer[1];

    case 0x20:	

        return s->xcer[0];

    case 0x22:	

        return s->xcer[1];

    case 0x24:	

        return s->pcr;

    case 0x26:	

        return s->rcer[2];

    case 0x28:	

        return s->rcer[3];

    case 0x2a:	

        return s->xcer[2];

    case 0x2c:	

        return s->xcer[3];

    case 0x2e:	

        return s->rcer[4];

    case 0x30:	

        return s->rcer[5];

    case 0x32:	

        return s->xcer[4];

    case 0x34:	

        return s->xcer[5];

    case 0x36:	

        return s->rcer[6];

    case 0x38:	

        return s->rcer[7];

    case 0x3a:	

        return s->xcer[6];

    case 0x3c:	

        return s->xcer[7];

    }



    OMAP_BAD_REG(addr);

    return 0;

}