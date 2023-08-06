void qemu_bh_schedule_idle(QEMUBH *bh)

{

    if (bh->scheduled)

        return;

    bh->idle = 1;

    

    smp_wmb();

    bh->scheduled = 1;

}