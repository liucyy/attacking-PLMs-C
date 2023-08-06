static void sunkbd_event(void *opaque, int ch)

{

    ChannelState *s = opaque;

    int release = ch & 0x80;



    trace_escc_sunkbd_event_in(ch);

    switch (ch) {

    case 58: 

        s->caps_lock_mode ^= 1;

        if (s->caps_lock_mode == 2)

            return; 

        break;

    case 69: 

        s->num_lock_mode ^= 1;

        if (s->num_lock_mode == 2)

            return; 

        break;

    case 186: 

        s->caps_lock_mode ^= 2;

        if (s->caps_lock_mode == 3)

            return; 

        break;

    case 197: 

        s->num_lock_mode ^= 2;

        if (s->num_lock_mode == 3)

            return; 

        break;

    case 0xe0:

        s->e0_mode = 1;

        return;

    default:

        break;

    }

    if (s->e0_mode) {

        s->e0_mode = 0;

        ch = e0_keycodes[ch & 0x7f];

    } else {

        ch = keycodes[ch & 0x7f];

    }

    trace_escc_sunkbd_event_out(ch);

    put_queue(s, ch | release);

}