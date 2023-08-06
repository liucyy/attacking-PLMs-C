static void qemu_mod_timer_ns(QEMUTimer *ts, int64_t expire_time)

{

    QEMUTimer **pt, *t;



    qemu_del_timer(ts);



    

    

    pt = &active_timers[ts->clock->type];

    for(;;) {

        t = *pt;

        if (!t)

            break;

        if (t->expire_time > expire_time)

            break;

        pt = &t->next;

    }

    ts->expire_time = expire_time;

    ts->next = *pt;

    *pt = ts;



    

    if (pt == &active_timers[ts->clock->type]) {

        if (!alarm_timer->pending) {

            qemu_rearm_alarm_timer(alarm_timer);

        }

        

        if (use_icount)

            qemu_notify_event();

    }

}