static uint64_t pl110_read(void *opaque, hwaddr offset,

                           unsigned size)

{

    pl110_state *s = (pl110_state *)opaque;



    if (offset >= 0xfe0 && offset < 0x1000) {

        return idregs[s->version][(offset - 0xfe0) >> 2];

    }

    if (offset >= 0x200 && offset < 0x400) {

        return s->raw_palette[(offset - 0x200) >> 2];

    }

    switch (offset >> 2) {

    case 0: 

        return s->timing[0];

    case 1: 

        return s->timing[1];

    case 2: 

        return s->timing[2];

    case 3: 

        return s->timing[3];

    case 4: 

        return s->upbase;

    case 5: 

        return s->lpbase;

    case 6: 

        if (s->version != PL110) {

            return s->cr;

        }

        return s->int_mask;

    case 7: 

        if (s->version != PL110) {

            return s->int_mask;

        }

        return s->cr;

    case 8: 

        return s->int_status;

    case 9: 

        return s->int_status & s->int_mask;

    case 11: 

        

        return s->upbase;

    case 12: 

        return s->lpbase;

    default:

        hw_error("pl110_read: Bad offset %x\n", (int)offset);

        return 0;

    }

}