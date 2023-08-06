static uint64_t imx_timerp_read(void *opaque, target_phys_addr_t offset,

                                unsigned size)

{

    IMXTimerPState *s = (IMXTimerPState *)opaque;



    DPRINTF("p-read(offset=%x)", offset >> 2);

    switch (offset >> 2) {

    case 0: 

        DPRINTF("cr %x\n", s->cr);

        return s->cr;



    case 1: 

        DPRINTF("int_level %x\n", s->int_level);

        return s->int_level;



    case 2: 

        DPRINTF("lr %x\n", s->lr);

        return s->lr;



    case 3: 

        DPRINTF("cmp %x\n", s->cmp);

        return s->cmp;



    case 4: 

        return ptimer_get_count(s->timer);

    }

    IPRINTF("imx_timerp_read: Bad offset %x\n",

            (int)offset >> 2);

    return 0;

}