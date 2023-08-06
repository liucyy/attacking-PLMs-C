int qemu_calculate_timeout(void)

{

#ifndef CONFIG_IOTHREAD

    int timeout;



    if (!vm_running)

        timeout = 5000;

    else {

     

        int64_t add;

        int64_t delta;

        

	delta = qemu_icount_delta();

        if (delta > 0) {

            

            timeout = (delta + 999999) / 1000000;

        } else {

            

            add = qemu_next_deadline();

            

            if (add > 10000000)

                add = 10000000;

            delta += add;

            qemu_icount += qemu_icount_round (add);

            timeout = delta / 1000000;

            if (timeout < 0)

                timeout = 0;

        }

    }



    return timeout;

#else 

    return 1000;

#endif

}