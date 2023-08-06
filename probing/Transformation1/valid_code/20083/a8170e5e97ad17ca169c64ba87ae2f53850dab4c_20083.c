static void imx_avic_write(void *opaque, target_phys_addr_t offset,

                          uint64_t val, unsigned size)

{

    IMXAVICState *s = (IMXAVICState *)opaque;



    

    if (offset >= 0x100 && offset <= 0x2fc) {

        IPRINTF("imx_avic_write to vector register %d ignored\n",

                (unsigned int)((offset - 0x100) >> 2));

        return;

    }



    DPRINTF("imx_avic_write(0x%x) = %x\n",

            (unsigned int)offset>>2, (unsigned int)val);

    switch (offset >> 2) {

    case 0: 

        s->intcntl = val & (ABFEN | NIDIS | FIDIS | NIAD | FIAD | NM);

        if (s->intcntl & ABFEN) {

            s->intcntl &= ~(val & ABFLAG);

        }

        break;



    case 1: 

        s->intmask = val & 0x1f;

        break;



    case 2: 

        DPRINTF("enable(%d)\n", (int)val);

        val &= 0x3f;

        s->enabled |= (1ULL << val);

        break;



    case 3: 

        DPRINTF("disable(%d)\n", (int)val);

        val &= 0x3f;

        s->enabled &= ~(1ULL << val);

        break;



    case 4: 

        s->enabled = (s->enabled & 0xffffffffULL) | (val << 32);

        break;



    case 5: 

        s->enabled = (s->enabled & 0xffffffff00000000ULL) | val;

        break;



    case 6: 

        s->is_fiq = (s->is_fiq & 0xffffffffULL) | (val << 32);

        break;



    case 7: 

        s->is_fiq = (s->is_fiq & 0xffffffff00000000ULL) | val;

        break;



    case 8: 

    case 9: 

    case 10:

    case 11:

    case 12:

    case 13:

    case 14:

    case 15:

        s->prio[15-(offset>>2)] = val;

        break;



        

    case 16:

    case 17:

    case 18:

    case 19:

        return;



    case 20:

        s->pending = (s->pending & 0xffffffffULL) | (val << 32);

        break;



    case 21:

        s->pending = (s->pending & 0xffffffff00000000ULL) | val;

        break;



    case 22:

    case 23:

    case 24: 

    case 25: 

        return;



    default:

        IPRINTF("imx_avic_write: Bad offset %x\n", (int)offset);

    }

    imx_avic_update(s);

}