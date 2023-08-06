static void intel_hda_update_int_sts(IntelHDAState *d)

{

    uint32_t sts = 0;

    uint32_t i;



    

    if (d->rirb_sts & ICH6_RBSTS_IRQ) {

        sts |= (1 << 30);

    }

    if (d->rirb_sts & ICH6_RBSTS_OVERRUN) {

        sts |= (1 << 30);

    }

    if (d->state_sts & d->wake_en) {

        sts |= (1 << 30);

    }



    

    for (i = 0; i < 8; i++) {

        

        if (d->st[i].ctl & (1 << 26)) {

            sts |= (1 << i);

        }

    }



    

    if (sts & d->int_ctl) {

        sts |= (1 << 31);

    }



    d->int_sts = sts;

}