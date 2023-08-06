static uint32_t grlib_irqmp_readl(void *opaque, target_phys_addr_t addr)

{

    IRQMP      *irqmp = opaque;

    IRQMPState *state;



    assert(irqmp != NULL);

    state = irqmp->state;

    assert(state != NULL);



    addr &= 0xff;



    

    switch (addr) {

    case LEVEL_OFFSET:

        return state->level;



    case PENDING_OFFSET:

        return state->pending;



    case FORCE0_OFFSET:

        

        return state->force[0];



    case CLEAR_OFFSET:

    case MP_STATUS_OFFSET:

        

        return 0;



    case BROADCAST_OFFSET:

        return state->broadcast;



    default:

        break;

    }



    

    if (addr >= MASK_OFFSET && addr < FORCE_OFFSET) {

        int cpu = (addr - MASK_OFFSET) / 4;

        assert(cpu >= 0 && cpu < IRQMP_MAX_CPU);



        return state->mask[cpu];

    }



    

    if (addr >= FORCE_OFFSET && addr < EXTENDED_OFFSET) {

        int cpu = (addr - FORCE_OFFSET) / 4;

        assert(cpu >= 0 && cpu < IRQMP_MAX_CPU);



        return state->force[cpu];

    }



    

    if (addr >= EXTENDED_OFFSET && addr < IRQMP_REG_SIZE) {

        int cpu = (addr - EXTENDED_OFFSET) / 4;

        assert(cpu >= 0 && cpu < IRQMP_MAX_CPU);



        return state->extended[cpu];

    }



    trace_grlib_irqmp_unknown_register("read", addr);

    return 0;

}