void main_loop_wait(int timeout)

{

    IOHandlerRecord *ioh;

    fd_set rfds, wfds, xfds;

    int ret, nfds;

    struct timeval tv;



    qemu_bh_update_timeout(&timeout);



    host_main_loop_wait(&timeout);



    

    

    nfds = -1;

    FD_ZERO(&rfds);

    FD_ZERO(&wfds);

    FD_ZERO(&xfds);

    for(ioh = first_io_handler; ioh != NULL; ioh = ioh->next) {

        if (ioh->deleted)

            continue;

        if (ioh->fd_read &&

            (!ioh->fd_read_poll ||

             ioh->fd_read_poll(ioh->opaque) != 0)) {

            FD_SET(ioh->fd, &rfds);

            if (ioh->fd > nfds)

                nfds = ioh->fd;

        }

        if (ioh->fd_write) {

            FD_SET(ioh->fd, &wfds);

            if (ioh->fd > nfds)

                nfds = ioh->fd;

        }

    }



    tv.tv_sec = timeout / 1000;

    tv.tv_usec = (timeout % 1000) * 1000;



    slirp_select_fill(&nfds, &rfds, &wfds, &xfds);



    qemu_mutex_unlock_iothread();

    ret = select(nfds + 1, &rfds, &wfds, &xfds, &tv);

    qemu_mutex_lock_iothread();

    if (ret > 0) {

        IOHandlerRecord **pioh;



        for(ioh = first_io_handler; ioh != NULL; ioh = ioh->next) {

            if (!ioh->deleted && ioh->fd_read && FD_ISSET(ioh->fd, &rfds)) {

                ioh->fd_read(ioh->opaque);

            }

            if (!ioh->deleted && ioh->fd_write && FD_ISSET(ioh->fd, &wfds)) {

                ioh->fd_write(ioh->opaque);

            }

        }



	

	pioh = &first_io_handler;

	while (*pioh) {

            ioh = *pioh;

            if (ioh->deleted) {

                *pioh = ioh->next;

                qemu_free(ioh);

            } else

                pioh = &ioh->next;

        }

    }



    slirp_select_poll(&rfds, &wfds, &xfds, (ret < 0));



    

    if (alarm_timer->flags & ALARM_FLAG_EXPIRED) {

        alarm_timer->flags &= ~ALARM_FLAG_EXPIRED;

        qemu_rearm_alarm_timer(alarm_timer);

    }



    

    if (vm_running) {

        if (!cur_cpu || likely(!(cur_cpu->singlestep_enabled & SSTEP_NOTIMER)))

            qemu_run_timers(&active_timers[QEMU_TIMER_VIRTUAL],

                qemu_get_clock(vm_clock));

    }



    

    qemu_run_timers(&active_timers[QEMU_TIMER_REALTIME],

                    qemu_get_clock(rt_clock));



    

    qemu_bh_poll();



}