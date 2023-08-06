static uint32_t omap_sysctl_read(void *opaque, target_phys_addr_t addr)

{

    struct omap_sysctl_s *s = (struct omap_sysctl_s *) opaque;



    switch (addr) {

    case 0x000:	

        return 0x20;



    case 0x010:	

        return s->sysconfig;



    case 0x030 ... 0x140:	

        return s->padconf[(addr - 0x30) >> 2];



    case 0x270:	

        return s->obs;



    case 0x274:	

        return s->devconfig;



    case 0x28c:	

        return 0;



    case 0x290:	

        return s->msuspendmux[0];

    case 0x294:	

        return s->msuspendmux[1];

    case 0x298:	

        return s->msuspendmux[2];

    case 0x29c:	

        return s->msuspendmux[3];

    case 0x2a0:	

        return s->msuspendmux[4];

    case 0x2a4:	

        return 0;



    case 0x2b8:	

        return s->psaconfig;

    case 0x2bc:	

    case 0x2c0:	

        return 0;



    case 0x2b0:	

        return 0x800000f1;

    case 0x2d0:	

        return 0x80000015;

    case 0x2d4:	

        return 0x8000007f;

    case 0x2b4:	

    case 0x2f0:	

    case 0x2f4:	

        

        return 0;



    case 0x2d8:	

        return 0xff;

    case 0x2dc:	

    case 0x2e0:	

    case 0x2e4:	

        

        return 0;



    case 0x2f8:	

        

        return 0x0300;

    case 0x2fc:	



    case 0x300:	

    case 0x304:	

    case 0x308:	

    case 0x30c:	

        return 0xdecafbad;



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

        

        return 0;

    }



    OMAP_BAD_REG(addr);

    return 0;

}