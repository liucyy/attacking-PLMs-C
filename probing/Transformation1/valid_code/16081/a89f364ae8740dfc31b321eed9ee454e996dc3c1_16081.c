static void pxa2xx_pm_write(void *opaque, hwaddr addr,

                            uint64_t value, unsigned size)

{

    PXA2xxState *s = (PXA2xxState *) opaque;



    switch (addr) {

    case PMCR:

        

        s->pm_regs[addr >> 2] &= ~(value & 0x2a);

        

        s->pm_regs[addr >> 2] &= ~0x15;

        s->pm_regs[addr >> 2] |= value & 0x15;

        break;



    case PSSR:	

    case RCSR:

    case PKSR:

        s->pm_regs[addr >> 2] &= ~value;

        break;



    default:	

        if (!(addr & 3)) {

            s->pm_regs[addr >> 2] = value;

            break;

        }



        printf("%s: Bad register " REG_FMT "\n", __FUNCTION__, addr);

        break;

    }

}