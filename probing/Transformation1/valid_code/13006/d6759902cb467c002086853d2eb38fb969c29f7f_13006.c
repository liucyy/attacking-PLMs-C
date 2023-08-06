static void arm_timer_write(void *opaque, target_phys_addr_t offset,

                            uint32_t value)

{

    arm_timer_state *s = (arm_timer_state *)opaque;

    int freq;



    switch (offset >> 2) {

    case 0: 

        s->limit = value;

        arm_timer_recalibrate(s, 1);

        break;

    case 1: 

        

        break;

    case 2: 

        if (s->control & TIMER_CTRL_ENABLE) {

            

            ptimer_stop(s->timer);

        }

        s->control = value;

        freq = s->freq;

        

        switch ((value >> 2) & 3) {

        case 1: freq >>= 4; break;

        case 2: freq >>= 8; break;

        }

        arm_timer_recalibrate(s, 0);

        ptimer_set_freq(s->timer, freq);

        if (s->control & TIMER_CTRL_ENABLE) {

            

            ptimer_run(s->timer, (s->control & TIMER_CTRL_ONESHOT) != 0);

        }

        break;

    case 3: 

        s->int_level = 0;

        break;

    case 6: 

        s->limit = value;

        arm_timer_recalibrate(s, 0);

        break;

    default:

        hw_error("arm_timer_write: Bad offset %x\n", (int)offset);

    }

    arm_timer_update(s);

}