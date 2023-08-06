static void timerlist_rearm(QEMUTimerList *timer_list)

{

    

    if (timer_list->clock->type == QEMU_CLOCK_VIRTUAL) {

        qemu_start_warp_timer();

    }

    timerlist_notify(timer_list);

}