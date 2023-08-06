static void slavio_timer_get_out(SLAVIO_TIMERState *s)

{

    int out;

    int64_t diff, ticks, count;

    uint32_t limit;



    

    

    if (s->mode == 1 && s->stopped)

	ticks = s->stop_time;

    else

	ticks = qemu_get_clock(vm_clock) - s->tick_offset;



    out = (ticks > s->expire_time);

    if (out)

	s->reached = 0x80000000;

    if (!s->limit)

	limit = 0x7fffffff;

    else

	limit = s->limit;



    

    limit = limit >> 9;



    

    diff = muldiv64(ticks - s->count_load_time, CNT_FREQ, ticks_per_sec);



    

    

    count = diff % limit;

    s->count = count << 9;

    s->counthigh = count >> 22;



    

    

    s->expire_time = ticks + muldiv64(limit - count, ticks_per_sec, CNT_FREQ);



    DPRINTF("irq %d limit %d reached %d d %" PRId64 " count %d s->c %x diff %" PRId64 " stopped %d mode %d\n", s->irq, limit, s->reached?1:0, (ticks-s->count_load_time), count, s->count, s->expire_time - ticks, s->stopped, s->mode);



    if (s->mode != 1)

	pic_set_irq_cpu(s->intctl, s->irq, out, s->cpu);

}