static void omap_disc_write(void *opaque, hwaddr addr,

                            uint64_t value, unsigned size)

{

    struct omap_dss_s *s = (struct omap_dss_s *) opaque;



    if (size != 4) {

        omap_badwidth_write32(opaque, addr, value);

        return;

    }



    switch (addr) {

    case 0x010:	

        if (value & 2)						

            omap_dss_reset(s);

        s->dispc.idlemode = value & 0x301b;

        break;



    case 0x018:	

        s->dispc.irqst &= ~value;

        omap_dispc_interrupt_update(s);

        break;



    case 0x01c:	

        s->dispc.irqen = value & 0xffff;

        omap_dispc_interrupt_update(s);

        break;



    case 0x040:	

        s->dispc.control = value & 0x07ff9fff;

        s->dig.enable = (value >> 1) & 1;

        s->lcd.enable = (value >> 0) & 1;

        if (value & (1 << 12))			

            if (!((s->dispc.l[1].attr | s->dispc.l[2].attr) & 1)) {

                fprintf(stderr, "%s: Overlay Optimization when no overlay "

                        "region effectively exists leads to "

                        "unpredictable behaviour!\n", __func__);

            }

        if (value & (1 << 6)) {				

            

        }

        if (value & (1 << 5)) {				

             

        }

        s->dispc.invalidate = 1;

        break;



    case 0x044:	

        s->dispc.config = value & 0x3fff;

        

        s->dispc.invalidate = 1;

        break;



    case 0x048:	

        s->dispc.capable = value & 0x3ff;

        break;



    case 0x04c:	

        s->dispc.bg[0] = value & 0xffffff;

        s->dispc.invalidate = 1;

        break;

    case 0x050:	

        s->dispc.bg[1] = value & 0xffffff;

        s->dispc.invalidate = 1;

        break;

    case 0x054:	

        s->dispc.trans[0] = value & 0xffffff;

        s->dispc.invalidate = 1;

        break;

    case 0x058:	

        s->dispc.trans[1] = value & 0xffffff;

        s->dispc.invalidate = 1;

        break;



    case 0x060:	

        s->dispc.line = value & 0x7ff;

        break;



    case 0x064:	

        s->dispc.timing[0] = value & 0x0ff0ff3f;

        break;

    case 0x068:	

        s->dispc.timing[1] = value & 0x0ff0ff3f;

        break;

    case 0x06c:	

        s->dispc.timing[2] = value & 0x0003ffff;

        break;

    case 0x070:	

        s->dispc.timing[3] = value & 0x00ff00ff;

        break;



    case 0x078:	

        s->dig.nx = ((value >>  0) & 0x7ff) + 1;		

        s->dig.ny = ((value >> 16) & 0x7ff) + 1;		

        s->dispc.invalidate = 1;

        break;

    case 0x07c:	

        s->lcd.nx = ((value >>  0) & 0x7ff) + 1;		

        s->lcd.ny = ((value >> 16) & 0x7ff) + 1;		

        s->dispc.invalidate = 1;

        break;

    case 0x080:	

        s->dispc.l[0].addr[0] = (hwaddr) value;

        s->dispc.invalidate = 1;

        break;

    case 0x084:	

        s->dispc.l[0].addr[1] = (hwaddr) value;

        s->dispc.invalidate = 1;

        break;

    case 0x088:	

        s->dispc.l[0].posx = ((value >>  0) & 0x7ff);		

        s->dispc.l[0].posy = ((value >> 16) & 0x7ff);		

        s->dispc.invalidate = 1;

        break;

    case 0x08c:	

        s->dispc.l[0].nx = ((value >>  0) & 0x7ff) + 1;		

        s->dispc.l[0].ny = ((value >> 16) & 0x7ff) + 1;		

        s->dispc.invalidate = 1;

        break;

    case 0x0a0:	

        s->dispc.l[0].attr = value & 0x7ff;

        if (value & (3 << 9))

            fprintf(stderr, "%s: Big-endian pixel format not supported\n",

                            __FUNCTION__);

        s->dispc.l[0].enable = value & 1;

        s->dispc.l[0].bpp = (value >> 1) & 0xf;

        s->dispc.invalidate = 1;

        break;

    case 0x0a4:	

        s->dispc.l[0].tresh = value & 0x01ff01ff;

        break;

    case 0x0ac:	

        s->dispc.l[0].rowinc = value;

        s->dispc.invalidate = 1;

        break;

    case 0x0b0:	

        s->dispc.l[0].colinc = value;

        s->dispc.invalidate = 1;

        break;

    case 0x0b4:	

        s->dispc.l[0].wininc = value;

        break;

    case 0x0b8:	

        s->dispc.l[0].addr[2] = (hwaddr) value;

        s->dispc.invalidate = 1;

        break;



    case 0x0bc:	

    case 0x0c0:	

    case 0x0c4:	

    case 0x0c8:	

    case 0x0cc:	

    case 0x0d0:	

    case 0x0d8:	

    case 0x0dc:	

    case 0x0e0:	

    case 0x0e4:	

    case 0x0e8:	

    case 0x0ec:	

    case 0x0f0 ... 0x140:	

    case 0x14c:	

    case 0x150:	

    case 0x154:	

    case 0x158:	

    case 0x15c:	

    case 0x160:	

    case 0x168:	

    case 0x16c:	

    case 0x170:	

    case 0x174:	

    case 0x178:	

    case 0x17c:	

    case 0x180 ... 0x1d0:	

    case 0x1d4:	

    case 0x1d8:	

    case 0x1dc:	

        break;



    default:

        OMAP_BAD_REG(addr);

    }

}