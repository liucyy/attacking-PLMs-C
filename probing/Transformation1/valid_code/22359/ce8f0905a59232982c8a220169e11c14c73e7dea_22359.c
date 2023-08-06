static void pl011_write(void *opaque, hwaddr offset,

                        uint64_t value, unsigned size)

{

    PL011State *s = (PL011State *)opaque;

    unsigned char ch;



    switch (offset >> 2) {

    case 0: 

        

        ch = value;

        if (s->chr)

            qemu_chr_fe_write(s->chr, &ch, 1);

        s->int_level |= PL011_INT_TX;

        pl011_update(s);

        break;

    case 1: 

        s->cr = value;

        break;

    case 6: 

        

        break;

    case 8: 

        s->ilpr = value;

        break;

    case 9: 

        s->ibrd = value;

        break;

    case 10: 

        s->fbrd = value;

        break;

    case 11: 

        

        if ((s->lcr ^ value) & 0x10) {

            s->read_count = 0;

            s->read_pos = 0;

        }

        s->lcr = value;

        pl011_set_read_trigger(s);

        break;

    case 12: 

        

        s->cr = value;

        break;

    case 13: 

        s->ifl = value;

        pl011_set_read_trigger(s);

        break;

    case 14: 

        s->int_enabled = value;

        pl011_update(s);

        break;

    case 17: 

        s->int_level &= ~value;

        pl011_update(s);

        break;

    case 18: 

        s->dmacr = value;

        if (value & 3) {

            qemu_log_mask(LOG_UNIMP, "pl011: DMA not implemented\n");

        }

        break;

    default:

        qemu_log_mask(LOG_GUEST_ERROR,

                      "pl011_write: Bad offset %x\n", (int)offset);

    }

}