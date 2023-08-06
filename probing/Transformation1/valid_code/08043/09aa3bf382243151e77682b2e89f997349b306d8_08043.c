static uint64_t pl061_read(void *opaque, hwaddr offset,

                           unsigned size)

{

    PL061State *s = (PL061State *)opaque;



    if (offset >= 0xfd0 && offset < 0x1000) {

        return s->id[(offset - 0xfd0) >> 2];

    }

    if (offset < 0x400) {

        return s->data & (offset >> 2);

    }

    switch (offset) {

    case 0x400: 

        return s->dir;

    case 0x404: 

        return s->isense;

    case 0x408: 

        return s->ibe;

    case 0x40c: 

        return s->iev;

    case 0x410: 

        return s->im;

    case 0x414: 

        return s->istate;

    case 0x418: 

        return s->istate & s->im;

    case 0x420: 

        return s->afsel;

    case 0x500: 

        return s->dr2r;

    case 0x504: 

        return s->dr4r;

    case 0x508: 

        return s->dr8r;

    case 0x50c: 

        return s->odr;

    case 0x510: 

        return s->pur;

    case 0x514: 

        return s->pdr;

    case 0x518: 

        return s->slr;

    case 0x51c: 

        return s->den;

    case 0x520: 

        return s->locked;

    case 0x524: 

        return s->cr;

    case 0x528: 

        return s->amsel;

    default:

        qemu_log_mask(LOG_GUEST_ERROR,

                      "pl061_read: Bad offset %x\n", (int)offset);

        return 0;

    }

}