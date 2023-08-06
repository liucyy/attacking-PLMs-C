static uint64_t pl011_read(void *opaque, hwaddr offset,

                           unsigned size)

{

    PL011State *s = (PL011State *)opaque;

    uint32_t c;



    if (offset >= 0xfe0 && offset < 0x1000) {

        return s->id[(offset - 0xfe0) >> 2];

    }

    switch (offset >> 2) {

    case 0: 

        s->flags &= ~PL011_FLAG_RXFF;

        c = s->read_fifo[s->read_pos];

        if (s->read_count > 0) {

            s->read_count--;

            if (++s->read_pos == 16)

                s->read_pos = 0;

        }

        if (s->read_count == 0) {

            s->flags |= PL011_FLAG_RXFE;

        }

        if (s->read_count == s->read_trigger - 1)

            s->int_level &= ~ PL011_INT_RX;

        pl011_update(s);

        if (s->chr) {

            qemu_chr_accept_input(s->chr);

        }

        return c;

    case 1: 

        return 0;

    case 6: 

        return s->flags;

    case 8: 

        return s->ilpr;

    case 9: 

        return s->ibrd;

    case 10: 

        return s->fbrd;

    case 11: 

        return s->lcr;

    case 12: 

        return s->cr;

    case 13: 

        return s->ifl;

    case 14: 

        return s->int_enabled;

    case 15: 

        return s->int_level;

    case 16: 

        return s->int_level & s->int_enabled;

    case 18: 

        return s->dmacr;

    default:

        qemu_log_mask(LOG_GUEST_ERROR,

                      "pl011_read: Bad offset %x\n", (int)offset);

        return 0;

    }

}