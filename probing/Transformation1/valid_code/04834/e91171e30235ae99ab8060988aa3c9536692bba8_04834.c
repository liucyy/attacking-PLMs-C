void ptimer_set_limit(ptimer_state *s, uint64_t limit, int reload)

{

    



    if (!use_icount && limit * s->period < 10000 && s->period) {

        limit = 10000 / s->period;

    }



    s->limit = limit;

    if (reload)

        s->delta = limit;

    if (s->enabled && reload) {

        s->next_event = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);

        ptimer_reload(s);

    }

}