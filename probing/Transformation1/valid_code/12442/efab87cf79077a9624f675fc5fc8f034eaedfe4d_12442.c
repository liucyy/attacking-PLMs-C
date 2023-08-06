void qemu_clock_warp(QEMUClockType type)

{

    int64_t clock;

    int64_t deadline;



    

    if (type != QEMU_CLOCK_VIRTUAL || !use_icount) {

        return;

    }



    if (icount_sleep) {

        

        icount_warp_rt(NULL);

        timer_del(icount_warp_timer);

    }

    if (!all_cpu_threads_idle()) {

        return;

    }



    if (qtest_enabled()) {

        

	return;

    }



    

    clock = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL_RT);

    deadline = qemu_clock_deadline_ns_all(QEMU_CLOCK_VIRTUAL);

    if (deadline < 0) {

        static bool notified;

        if (!icount_sleep && !notified) {

            error_report("WARNING: icount sleep disabled and no active timers");

            notified = true;

        }

        return;

    }



    if (deadline > 0) {

        

        if (!icount_sleep) {

            

            seqlock_write_lock(&timers_state.vm_clock_seqlock);

            timers_state.qemu_icount_bias += deadline;

            seqlock_write_unlock(&timers_state.vm_clock_seqlock);

            qemu_clock_notify(QEMU_CLOCK_VIRTUAL);

        } else {

            

            seqlock_write_lock(&timers_state.vm_clock_seqlock);

            if (vm_clock_warp_start == -1 || vm_clock_warp_start > clock) {

                vm_clock_warp_start = clock;

            }

            seqlock_write_unlock(&timers_state.vm_clock_seqlock);

            timer_mod_anticipate(icount_warp_timer, clock + deadline);

        }

    } else if (deadline == 0) {

        qemu_clock_notify(QEMU_CLOCK_VIRTUAL);

    }

}