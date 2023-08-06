static uint32_t arm_timer_read(void *opaque, target_phys_addr_t offset)

{

    arm_timer_state *s = (arm_timer_state *)opaque;



    switch (offset >> 2) {

    case 0: 

    case 6: 

        return s->limit;

    case 1: 

        return ptimer_get_count(s->timer);

    case 2: 

        return s->control;

    case 4: 

        return s->int_level;

    case 5: 

        if ((s->control & TIMER_CTRL_IE) == 0)

            return 0;

        return s->int_level;

    default:

        hw_error("%s: Bad offset %x\n", __func__, (int)offset);

        return 0;

    }

}