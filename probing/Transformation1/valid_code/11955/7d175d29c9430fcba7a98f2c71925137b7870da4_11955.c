static int os_host_main_loop_wait(int64_t timeout)

{

    int ret;

    static int spin_counter;



    glib_pollfds_fill(&timeout);



    

    if (!timeout && (spin_counter > MAX_MAIN_LOOP_SPIN)) {

        static bool notified;



        if (!notified && !qtest_driver()) {

            fprintf(stderr,

                    "main-loop: WARNING: I/O thread spun for %d iterations\n",

                    MAX_MAIN_LOOP_SPIN);

            notified = true;

        }



        timeout = SCALE_MS;

    }



    if (timeout) {

        spin_counter = 0;

        qemu_mutex_unlock_iothread();

    } else {

        spin_counter++;

    }



    ret = qemu_poll_ns((GPollFD *)gpollfds->data, gpollfds->len, timeout);



    if (timeout) {

        qemu_mutex_lock_iothread();

    }



    glib_pollfds_poll();

    return ret;

}