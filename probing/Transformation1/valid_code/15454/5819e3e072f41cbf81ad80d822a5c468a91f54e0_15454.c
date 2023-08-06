gdb_handlesig(CPUState *cpu, int sig)

{

    GDBState *s;

    char buf[256];

    int n;



    s = gdbserver_state;

    if (gdbserver_fd < 0 || s->fd < 0) {

        return sig;

    }



    

    cpu_single_step(cpu, 0);

    tb_flush(cpu);



    if (sig != 0) {

        snprintf(buf, sizeof(buf), "S%02x", target_signal_to_gdb(sig));

        put_packet(s, buf);

    }

    

    if (s->fd < 0) {

        return sig;

    }



    sig = 0;

    s->state = RS_IDLE;

    s->running_state = 0;

    while (s->running_state == 0) {

        n = read(s->fd, buf, 256);

        if (n > 0) {

            int i;



            for (i = 0; i < n; i++) {

                gdb_read_byte(s, buf[i]);

            }

        } else if (n == 0 || errno != EAGAIN) {

            

            return sig;

        }

    }

    sig = s->signal;

    s->signal = 0;

    return sig;

}