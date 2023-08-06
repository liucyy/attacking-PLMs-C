static void hpet_reset(void *opaque) {

    HPETState *s = opaque;

    int i;

    static int count = 0;



    for (i=0; i<HPET_NUM_TIMERS; i++) {

        HPETTimer *timer = &s->timer[i];

        hpet_del_timer(timer);

        timer->tn = i;

        timer->cmp = ~0ULL;

        timer->config =  HPET_TN_PERIODIC_CAP | HPET_TN_SIZE_CAP;

        

        timer->config |=  0x00000004ULL << 32;

        timer->state = s;

        timer->period = 0ULL;

        timer->wrap_flag = 0;

    }



    s->hpet_counter = 0ULL;

    s->hpet_offset = 0ULL;

    

    s->capability = 0x8086a201ULL;

    s->capability |= ((HPET_CLK_PERIOD) << 32);

    s->config = 0ULL;

    if (count > 0)

        

        hpet_pit_enable();

    count = 1;

}