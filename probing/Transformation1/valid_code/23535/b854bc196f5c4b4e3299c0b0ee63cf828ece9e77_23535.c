static int omap_dma_ch_reg_write(struct omap_dma_s *s,

                int ch, int reg, uint16_t value) {

    switch (reg) {

    case 0x00:	

        s->ch[ch].burst[1] = (value & 0xc000) >> 14;

        s->ch[ch].pack[1] = (value & 0x2000) >> 13;

        s->ch[ch].port[1] = (enum omap_dma_port) ((value & 0x1e00) >> 9);

        s->ch[ch].burst[0] = (value & 0x0180) >> 7;

        s->ch[ch].pack[0] = (value & 0x0040) >> 6;

        s->ch[ch].port[0] = (enum omap_dma_port) ((value & 0x003c) >> 2);

        s->ch[ch].data_type = (1 << (value & 3));

        if (s->ch[ch].port[0] >= omap_dma_port_last)

            printf("%s: invalid DMA port %i\n", __FUNCTION__,

                            s->ch[ch].port[0]);

        if (s->ch[ch].port[1] >= omap_dma_port_last)

            printf("%s: invalid DMA port %i\n", __FUNCTION__,

                            s->ch[ch].port[1]);

        if ((value & 3) == 3)

            printf("%s: bad data_type for DMA channel %i\n", __FUNCTION__, ch);

        break;



    case 0x02:	

        s->ch[ch].mode[1] = (omap_dma_addressing_t) ((value & 0xc000) >> 14);

        s->ch[ch].mode[0] = (omap_dma_addressing_t) ((value & 0x3000) >> 12);

        s->ch[ch].end_prog = (value & 0x0800) >> 11;

        s->ch[ch].repeat = (value & 0x0200) >> 9;

        s->ch[ch].auto_init = (value & 0x0100) >> 8;

        s->ch[ch].priority = (value & 0x0040) >> 6;

        s->ch[ch].fs = (value & 0x0020) >> 5;

        s->ch[ch].sync = value & 0x001f;

        if (value & 0x0080) {

            if (s->ch[ch].running) {

                if (!s->ch[ch].signalled &&

                                s->ch[ch].auto_init && s->ch[ch].end_prog)

                    omap_dma_channel_load(s, ch);

            } else {

                s->ch[ch].running = 1;

                omap_dma_channel_load(s, ch);

            }

            if (!s->ch[ch].sync || (s->drq & (1 << s->ch[ch].sync)))

                omap_dma_request_run(s, ch, 0);

        } else {

            s->ch[ch].running = 0;

            omap_dma_request_stop(s, ch);

        }

        break;



    case 0x04:	

        s->ch[ch].interrupts = value & 0x003f;

        break;



    case 0x06:	

        return 1;



    case 0x08:	

        s->ch[ch].addr[0] &= 0xffff0000;

        s->ch[ch].addr[0] |= value;

        break;



    case 0x0a:	

        s->ch[ch].addr[0] &= 0x0000ffff;

        s->ch[ch].addr[0] |= value << 16;

        break;



    case 0x0c:	

        s->ch[ch].addr[1] &= 0xffff0000;

        s->ch[ch].addr[1] |= value;

        break;



    case 0x0e:	

        s->ch[ch].addr[1] &= 0x0000ffff;

        s->ch[ch].addr[1] |= value << 16;

        break;



    case 0x10:	

        s->ch[ch].elements = value & 0xffff;

        break;



    case 0x12:	

        s->ch[ch].frames = value & 0xffff;

        break;



    case 0x14:	

        s->ch[ch].frame_index = value & 0xffff;

        break;



    case 0x16:	

        s->ch[ch].element_index = value & 0xffff;

        break;



    case 0x18:	

        return 1;



    default:

        OMAP_BAD_REG((unsigned long) reg);

    }

    return 0;

}