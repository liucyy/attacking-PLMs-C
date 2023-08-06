static void pl061_write(void *opaque, target_phys_addr_t offset,

                        uint32_t value)

{

    pl061_state *s = (pl061_state *)opaque;

    uint8_t mask;



    if (offset < 0x400) {

        mask = (offset >> 2) & s->dir;

        s->data = (s->data & ~mask) | (value & mask);

        pl061_update(s);

        return;

    }

    switch (offset) {

    case 0x400: 

        s->dir = value;

        break;

    case 0x404: 

        s->isense = value;

        break;

    case 0x408: 

        s->ibe = value;

        break;

    case 0x40c: 

        s->iev = value;

        break;

    case 0x410: 

        s->im = value;

        break;

    case 0x41c: 

        s->istate &= ~value;

        break;

    case 0x420: 

        mask = s->cr;

        s->afsel = (s->afsel & ~mask) | (value & mask);

        break;

    case 0x500: 

        s->dr2r = value;

        break;

    case 0x504: 

        s->dr4r = value;

        break;

    case 0x508: 

        s->dr8r = value;

        break;

    case 0x50c: 

        s->odr = value;

        break;

    case 0x510: 

        s->pur = value;

        break;

    case 0x514: 

        s->pdr = value;

        break;

    case 0x518: 

        s->slr = value;

        break;

    case 0x51c: 

        s->den = value;

        break;

    case 0x520: 

        s->locked = (value != 0xacce551);

        break;

    case 0x524: 

        if (!s->locked)

            s->cr = value;

        break;

    default:

        hw_error("pl061_write: Bad offset %x\n", (int)offset);

    }

    pl061_update(s);

}