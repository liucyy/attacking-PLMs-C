void qemu_clock_warp(QEMUClockType type)

{

    int64_t deadline;



    

    if (type != QEMU_CLOCK_VIRTUAL || !use_icount) {

        return;

    }



    

    icount_warp_rt(NULL);

    if (!all_cpu_threads_idle() || !qemu_clock_has_timers(QEMU_CLOCK_VIRTUAL)) {

        timer_del(icount_warp_timer);

        return;

    }



    if (qtest_enabled()) {

        

	return;

    }



    vm_clock_warp_start = qemu_clock_get_ns(QEMU_CLOCK_REALTIME);

    

    deadline = qemu_clock_deadline_ns_all(QEMU_CLOCK_VIRTUAL);



    

    if ((deadline < 0) || (deadline > INT32_MAX)) {

        deadline = INT32_MAX;

    }



    if (deadline > 0) {

        

        timer_mod(icount_warp_timer, vm_clock_warp_start + deadline);

    } else if (deadline == 0) {

        qemu_clock_notify(QEMU_CLOCK_VIRTUAL);

    }

}