static uint64_t imx_avic_read(void *opaque,

                             target_phys_addr_t offset, unsigned size)

{

    IMXAVICState *s = (IMXAVICState *)opaque;





    DPRINTF("read(offset = 0x%x)\n", offset >> 2);

    switch (offset >> 2) {

    case 0: 

        return s->intcntl;



    case 1: 

        return s->intmask;



    case 2: 

    case 3: 

        return 0;



    case 4: 

        return s->enabled >> 32;



    case 5: 

        return s->enabled & 0xffffffffULL;



    case 6: 

        return s->is_fiq >> 32;



    case 7: 

        return s->is_fiq & 0xffffffffULL;



    case 8: 

    case 9: 

    case 10:

    case 11:

    case 12:

    case 13:

    case 14:

    case 15:

        return s->prio[15-(offset>>2)];



    case 16: 

    {

        

        uint64_t flags = s->pending & s->enabled & ~s->is_fiq;

        int i;

        int prio = -1;

        int irq = -1;

        for (i = 63; i >= 0; --i) {

            if (flags & (1ULL<<i)) {

                int irq_prio = imx_avic_prio(s, i);

                if (irq_prio > prio) {

                    irq = i;

                    prio = irq_prio;

                }

            }

        }

        if (irq >= 0) {

            imx_avic_set_irq(s, irq, 0);

            return irq << 16 | prio;

        }

        return 0xffffffffULL;

    }

    case 17:

    {

        uint64_t flags = s->pending & s->enabled & s->is_fiq;

        int i = ctz64(flags);

        if (i < 64) {

            imx_avic_set_irq(opaque, i, 0);

            return i;

        }

        return 0xffffffffULL;

    }

    case 18:

        return s->pending >> 32;



    case 19:

        return s->pending & 0xffffffffULL;



    case 20:

    case 21:

        return 0;



    case 22:

        return (s->pending & s->enabled & ~s->is_fiq) >> 32;



    case 23:

        return (s->pending & s->enabled & ~s->is_fiq) & 0xffffffffULL;



    case 24: 

        return (s->pending & s->enabled & s->is_fiq) >> 32;



    case 25: 

        return (s->pending & s->enabled & s->is_fiq) & 0xffffffffULL;



    case 0x40:            

        return 0x4;



    default:

        IPRINTF("imx_avic_read: Bad offset 0x%x\n", (int)offset);

        return 0;

    }

}