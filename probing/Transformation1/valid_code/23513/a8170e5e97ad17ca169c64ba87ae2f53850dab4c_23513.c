static void grlib_irqmp_write(void *opaque, target_phys_addr_t addr,

                              uint64_t value, unsigned size)

{

    IRQMP      *irqmp = opaque;

    IRQMPState *state;



    assert(irqmp != NULL);

    state = irqmp->state;

    assert(state != NULL);



    addr &= 0xff;



    

    switch (addr) {

    case LEVEL_OFFSET:

        value &= 0xFFFF << 1; 

        state->level = value;

        return;



    case PENDING_OFFSET:

        

        return;



    case FORCE0_OFFSET:

        



        value &= 0xFFFE; 

        state->force[0] = value;

        grlib_irqmp_check_irqs(irqmp->state);

        return;



    case CLEAR_OFFSET:

        value &= ~1; 

        state->pending &= ~value;

        return;



    case MP_STATUS_OFFSET:

        

        return;



    case BROADCAST_OFFSET:

        value &= 0xFFFE; 

        state->broadcast = value;

        return;



    default:

        break;

    }



    

    if (addr >= MASK_OFFSET && addr < FORCE_OFFSET) {

        int cpu = (addr - MASK_OFFSET) / 4;

        assert(cpu >= 0 && cpu < IRQMP_MAX_CPU);



        value &= ~1; 

        state->mask[cpu] = value;

        grlib_irqmp_check_irqs(irqmp->state);

        return;

    }



    

    if (addr >= FORCE_OFFSET && addr < EXTENDED_OFFSET) {

        int cpu = (addr - FORCE_OFFSET) / 4;

        assert(cpu >= 0 && cpu < IRQMP_MAX_CPU);



        uint32_t force = value & 0xFFFE;

        uint32_t clear = (value >> 16) & 0xFFFE;

        uint32_t old   = state->force[cpu];



        state->force[cpu] = (old | force) & ~clear;

        grlib_irqmp_check_irqs(irqmp->state);

        return;

    }



    

    if (addr >= EXTENDED_OFFSET && addr < IRQMP_REG_SIZE) {

        int cpu = (addr - EXTENDED_OFFSET) / 4;

        assert(cpu >= 0 && cpu < IRQMP_MAX_CPU);



        value &= 0xF; 

        state->extended[cpu] = value;

        return;

    }



    trace_grlib_irqmp_writel_unknown(addr, value);

}