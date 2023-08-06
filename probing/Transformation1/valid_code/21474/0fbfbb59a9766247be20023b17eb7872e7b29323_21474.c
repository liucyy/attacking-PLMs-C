int apic_get_interrupt(DeviceState *d)

{

    APICState *s = DO_UPCAST(APICState, busdev.qdev, d);

    int intno;



    

    if (!s)

        return -1;

    if (!(s->spurious_vec & APIC_SV_ENABLE))

        return -1;



    

    intno = get_highest_priority_int(s->irr);

    if (intno < 0)

        return -1;

    if (s->tpr && intno <= s->tpr)

        return s->spurious_vec & 0xff;

    reset_bit(s->irr, intno);

    set_bit(s->isr, intno);

    apic_update_irq(s);

    return intno;

}