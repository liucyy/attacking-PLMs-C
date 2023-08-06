static void imx_epit_reset(DeviceState *dev)

{

    IMXEPITState *s = IMX_EPIT(dev);



    

    s->cr &= (CR_EN|CR_ENMOD|CR_STOPEN|CR_DOZEN|CR_WAITEN|CR_DBGEN);

    s->sr = 0;

    s->lr = TIMER_MAX;

    s->cmp = 0;

    s->cnt = 0;

    

    ptimer_stop(s->timer_cmp);

    ptimer_stop(s->timer_reload);

    

    imx_epit_set_freq(s);

    

    ptimer_set_limit(s->timer_cmp, TIMER_MAX, 1);

    ptimer_set_limit(s->timer_reload, TIMER_MAX, 1);

    if (s->freq && (s->cr & CR_EN)) {

        

        ptimer_run(s->timer_reload, 0);

    }

}