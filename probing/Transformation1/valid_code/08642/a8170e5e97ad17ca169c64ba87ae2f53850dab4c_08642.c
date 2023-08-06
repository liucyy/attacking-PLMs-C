static uint64_t omap_prcm_read(void *opaque, target_phys_addr_t addr,

                               unsigned size)

{

    struct omap_prcm_s *s = (struct omap_prcm_s *) opaque;

    uint32_t ret;



    if (size != 4) {

        return omap_badwidth_read32(opaque, addr);

    }



    switch (addr) {

    case 0x000:	

        return 0x10;



    case 0x010:	

        return s->sysconfig;



    case 0x018:	

        return s->irqst[0];



    case 0x01c:	

        return s->irqen[0];



    case 0x050:	

        return s->voltctrl;

    case 0x054:	

        return s->voltctrl & 3;



    case 0x060:	

        return s->clksrc[0];

    case 0x070:	

        return s->clkout[0];

    case 0x078:	

        return s->clkemul[0];

    case 0x080:	

    case 0x084:	

        return 0;



    case 0x090:	

        return s->setuptime[0];



    case 0x094:	

        return s->setuptime[1];



    case 0x098:	

        return s->clkpol[0];



    case 0x0b0:	

    case 0x0b4:	

    case 0x0b8:	

    case 0x0bc:	

    case 0x0c0:	

    case 0x0c4:	

    case 0x0c8:	

    case 0x0cc:	

    case 0x0d0:	

    case 0x0d4:	

    case 0x0d8:	

    case 0x0dc:	

    case 0x0e0:	

    case 0x0e4:	

    case 0x0e8:	

    case 0x0ec:	

    case 0x0f0:	

    case 0x0f4:	

    case 0x0f8:	

    case 0x0fc:	

        return s->scratch[(addr - 0xb0) >> 2];



    case 0x140:	

        return s->clksel[0];

    case 0x148:	

        return s->clkctrl[0];



    case 0x158:	

        return s->rst[0];

    case 0x1c8:	

        return s->wkup[0];

    case 0x1d4:	

        return s->ev;

    case 0x1d8:	

        return s->evtime[0];

    case 0x1dc:	

        return s->evtime[1];

    case 0x1e0:	

        return s->power[0];

    case 0x1e4:	

        return 0;



    case 0x200:	

        return s->clken[0];

    case 0x204:	

        return s->clken[1];

    case 0x210:	

        return s->clken[2];

    case 0x214:	

        return s->clken[3];

    case 0x21c:	

        return s->clken[4];



    case 0x220:	

        

        return 0x7ffffff9;

    case 0x224:	

        

        return 0x00000007;

    case 0x22c:	

        

        return 0x0000001f;



    case 0x230:	

        return s->clkidle[0];

    case 0x234:	

        return s->clkidle[1];

    case 0x238:	

        return s->clkidle[2];

    case 0x23c:	

        return s->clkidle[3];



    case 0x240:	

        return s->clksel[1];

    case 0x244:	

        return s->clksel[2];



    case 0x248:	

        return s->clkctrl[1];



    case 0x2a0:	

        return s->wken[0];

    case 0x2a4:	

        return s->wken[1];



    case 0x2b0:	

        return s->wkst[0];

    case 0x2b4:	

        return s->wkst[1];

    case 0x2c8:	

        return 0x1e;



    case 0x2e0:	

        return s->power[1];

    case 0x2e4:	

        return 0x000030 | (s->power[1] & 0xfc00);



    case 0x300:	

        return s->clken[5];

    case 0x310:	

        return s->clken[6];

    case 0x320:	

        

        return 0x00000001;

    case 0x340:	

        return s->clksel[3];

    case 0x348:	

        return s->clkctrl[2];

    case 0x350:	

        return s->rstctrl[0];

    case 0x358:	

        return s->rst[1];

    case 0x3c8:	

        return s->wkup[1];



    case 0x3e0:	

        return s->power[2];

    case 0x3e4:	

        return s->power[2] & 3;



    case 0x400:	

        return s->clken[7];

    case 0x410:	

        return s->clken[8];

    case 0x420:	

        

        return 0x0000003f;

    case 0x430:	

        return s->clkidle[4];

    case 0x440:	

        return s->clksel[4];

    case 0x450:	

        return 0;

    case 0x454:	

        return s->rsttime_wkup;

    case 0x458:	

        return s->rst[2];

    case 0x4a0:	

        return s->wken[2];

    case 0x4b0:	

        return s->wkst[2];



    case 0x500:	

        return s->clken[9];

    case 0x520:	

        ret = 0x0000070 | (s->apll_lock[0] << 9) | (s->apll_lock[1] << 8);

        if (!(s->clksel[6] & 3))

            

            ret |= 3 << 0;

        else if (!s->dpll_lock)

            

            ret |= 1 << 0;

        else

            

            ret |= 2 << 0;

        return ret;

    case 0x530:	

        return s->clkidle[5];

    case 0x540:	

        return s->clksel[5];

    case 0x544:	

        return s->clksel[6];



    case 0x800:	

        return s->clken[10];

    case 0x810:	

        return s->clken[11];

    case 0x820:	

        

        return 0x00000103;

    case 0x830:	

        return s->clkidle[6];

    case 0x840:	

        return s->clksel[7];

    case 0x848:	

        return s->clkctrl[3];

    case 0x850:	

        return 0;

    case 0x858:	

        return s->rst[3];

    case 0x8c8:	

        return s->wkup[2];

    case 0x8e0:	

        return s->power[3];

    case 0x8e4:	

        return 0x008030 | (s->power[3] & 0x3003);



    case 0x8f0:	

        return s->irqst[1];

    case 0x8f4:	

        return s->irqen[1];



    case 0x8f8:	

        return s->irqst[2];

    case 0x8fc:	

        return s->irqen[2];

    }



    OMAP_BAD_REG(addr);

    return 0;

}