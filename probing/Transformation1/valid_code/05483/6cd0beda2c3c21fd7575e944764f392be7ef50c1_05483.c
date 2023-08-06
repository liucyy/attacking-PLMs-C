static int ram_save_iterate(QEMUFile *f, void *opaque)

{

    int ret;

    int i;

    int64_t t0;

    int total_sent = 0;



    qemu_mutex_lock_ramlist();



    if (ram_list.version != last_version) {

        reset_ram_globals();

    }



    ram_control_before_iterate(f, RAM_CONTROL_ROUND);



    t0 = qemu_clock_get_ns(QEMU_CLOCK_REALTIME);

    i = 0;

    while ((ret = qemu_file_rate_limit(f)) == 0) {

        int bytes_sent;



        bytes_sent = ram_save_block(f, false);

        

        if (bytes_sent == 0) {

            break;

        }

        total_sent += bytes_sent;

        acct_info.iterations++;

        check_guest_throttling();

        

        if ((i & 63) == 0) {

            uint64_t t1 = (qemu_clock_get_ns(QEMU_CLOCK_REALTIME) - t0) / 1000000;

            if (t1 > MAX_WAIT) {

                DPRINTF("big wait: %" PRIu64 " milliseconds, %d iterations\n",

                        t1, i);

                break;

            }

        }

        i++;

    }



    qemu_mutex_unlock_ramlist();



    

    ram_control_after_iterate(f, RAM_CONTROL_ROUND);



    if (ret < 0) {

        bytes_transferred += total_sent;

        return ret;

    }



    qemu_put_be64(f, RAM_SAVE_FLAG_EOS);

    total_sent += 8;

    bytes_transferred += total_sent;



    return total_sent;

}