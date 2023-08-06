uint64_t ptimer_get_count(ptimer_state *s)

{

    int64_t now;

    uint64_t counter;



    if (s->enabled) {

        now = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);

        

        if (now - s->next_event > 0

            || s->period == 0) {

            

            counter = 0;

        } else {

            uint64_t rem;

            uint64_t div;

            int clz1, clz2;

            int shift;



            



            rem = s->next_event - now;

            div = s->period;



            clz1 = clz64(rem);

            clz2 = clz64(div);

            shift = clz1 < clz2 ? clz1 : clz2;



            rem <<= shift;

            div <<= shift;

            if (shift >= 32) {

                div |= ((uint64_t)s->period_frac << (shift - 32));

            } else {

                if (shift != 0)

                    div |= (s->period_frac >> (32 - shift));

                

                if ((uint32_t)(s->period_frac << shift))

                    div += 1;

            }

            counter = rem / div;

        }

    } else {

        counter = s->delta;

    }

    return counter;

}