static void update_irq(struct HPETTimer *timer)

{

    qemu_irq irq;

    int route;



    if (timer->tn <= 1 && hpet_in_legacy_mode()) {

        

        if (timer->tn == 0) {

            irq=timer->state->irqs[0];

        } else

            irq=timer->state->irqs[8];

    } else {

        route=timer_int_route(timer);

        irq=timer->state->irqs[route];

    }

    if (timer_enabled(timer) && hpet_enabled()) {

        qemu_irq_pulse(irq);

    }

}