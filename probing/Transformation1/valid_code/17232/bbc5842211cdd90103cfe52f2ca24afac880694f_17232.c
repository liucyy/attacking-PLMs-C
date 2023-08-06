static void mpic_reset (void *opaque)

{

    openpic_t *mpp = (openpic_t *)opaque;

    int i;



    mpp->glbc = 0x80000000;

    

    mpp->frep = 0x004f0002;

    mpp->veni = VENI;

    mpp->pint = 0x00000000;

    mpp->spve = 0x0000FFFF;

    

    for (i = 0; i < mpp->max_irq; i++) {

        mpp->src[i].ipvp = 0x80800000;

        mpp->src[i].ide  = 0x00000001;

    }

    

    for (i = mpp->irq_ipi0; i < (mpp->irq_ipi0 + MAX_IPI); i++) {

        mpp->src[i].ide = 0;

    }

    

    for (i = 0; i < MAX_CPU; i++) {

        mpp->dst[i].pctp      = 0x0000000F;

        mpp->dst[i].tfrr      = 0x00000000;

        memset(&mpp->dst[i].raised, 0, sizeof(IRQ_queue_t));

        mpp->dst[i].raised.next = -1;

        memset(&mpp->dst[i].servicing, 0, sizeof(IRQ_queue_t));

        mpp->dst[i].servicing.next = -1;

    }

    

    for (i = 0; i < MAX_TMR; i++) {

        mpp->timers[i].ticc = 0x00000000;

        mpp->timers[i].tibc = 0x80000000;

    }

    

    mpp->glbc = 0x00000000;

}