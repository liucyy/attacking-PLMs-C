static void hpet_set_timer(HPETTimer *t)

{

    uint64_t diff;

    uint32_t wrap_diff;  

    uint64_t cur_tick = hpet_get_ticks();



    

    t->wrap_flag = 0;

    diff = hpet_calculate_diff(t, cur_tick);



    

    if (t->config & HPET_TN_32BIT && !timer_is_periodic(t)) {

        wrap_diff = 0xffffffff - (uint32_t)cur_tick;

        if (wrap_diff < (uint32_t)diff) {

            diff = wrap_diff;

            t->wrap_flag = 1;

        }

    }

    qemu_mod_timer(t->qemu_timer, qemu_get_clock(vm_clock)

                   + (int64_t)ticks_to_ns(diff));

}