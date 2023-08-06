static void omap_sysctl_write(void *opaque, target_phys_addr_t addr,

                uint32_t value)

{

    struct omap_sysctl_s *s = (struct omap_sysctl_s *) opaque;



    switch (addr) {

    case 0x000:	

    case 0x2a4:	

    case 0x2c0:	

    case 0x2f8:	

    case 0x2fc:	

    case 0x300:	

    case 0x304:	

    case 0x308:	

    case 0x30c:	

    case 0x310:	

    case 0x314:	

    case 0x318:	

    case 0x31c:	

    case 0x320:	

    case 0x324:	

    case 0x330:	

    case 0x334:	

    case 0x338:	

    case 0x33c:	

    case 0x340:	

    case 0x344:	

    case 0x348:	

    case 0x34c:	

    case 0x350:	

    case 0x354:	

        OMAP_RO_REG(addr);

        return;



    case 0x010:	

        s->sysconfig = value & 0x1e;

        break;



    case 0x030 ... 0x140:	

        

        s->padconf[(addr - 0x30) >> 2] = value & 0x1f1f1f1f;

        break;



    case 0x270:	

        s->obs = value & 0xff;

        break;



    case 0x274:	

        s->devconfig = value & 0xffffc7ff;

        break;



    case 0x28c:	

        break;



    case 0x290:	

        s->msuspendmux[0] = value & 0x3fffffff;

        break;

    case 0x294:	

        s->msuspendmux[1] = value & 0x3fffffff;

        break;

    case 0x298:	

        s->msuspendmux[2] = value & 0x3fffffff;

        break;

    case 0x29c:	

        s->msuspendmux[3] = value & 0x3fffffff;

        break;

    case 0x2a0:	

        s->msuspendmux[4] = value & 0x3fffffff;

        break;



    case 0x2b8:	

        s->psaconfig = value & 0x1c;

        s->psaconfig |= (value & 0x20) ? 2 : 1;

        break;

    case 0x2bc:	

        break;



    case 0x2b0:	

    case 0x2b4:	

    case 0x2d0:	

    case 0x2d4:	

    case 0x2d8:	

    case 0x2dc:	

    case 0x2e0:	

    case 0x2e4:	

    case 0x2f0:	

    case 0x2f4:	

        break;



    default:

        OMAP_BAD_REG(addr);

        return;

    }

}