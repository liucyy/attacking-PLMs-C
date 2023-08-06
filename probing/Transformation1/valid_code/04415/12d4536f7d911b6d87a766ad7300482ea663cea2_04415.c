void configure_icount(const char *option)

{

    vmstate_register(NULL, 0, &vmstate_timers, &timers_state);

    if (!option)

        return;



#ifdef CONFIG_IOTHREAD

    vm_clock->warp_timer = qemu_new_timer_ns(rt_clock, icount_warp_rt, NULL);

#endif



    if (strcmp(option, "auto") != 0) {

        icount_time_shift = strtol(option, NULL, 0);

        use_icount = 1;

        return;

    }



    use_icount = 2;



    

    icount_time_shift = 3;



    

    icount_rt_timer = qemu_new_timer_ms(rt_clock, icount_adjust_rt, NULL);

    qemu_mod_timer(icount_rt_timer,

                   qemu_get_clock_ms(rt_clock) + 1000);

    icount_vm_timer = qemu_new_timer_ns(vm_clock, icount_adjust_vm, NULL);

    qemu_mod_timer(icount_vm_timer,

                   qemu_get_clock_ns(vm_clock) + get_ticks_per_sec() / 10);

}