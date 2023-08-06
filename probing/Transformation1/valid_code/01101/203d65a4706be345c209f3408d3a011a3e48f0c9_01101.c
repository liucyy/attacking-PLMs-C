static void imx_epit_reload_compare_timer(IMXEPITState *s)

{

    if ((s->cr & (CR_EN | CR_OCIEN)) == (CR_EN | CR_OCIEN))  {

        

        uint32_t tmp = imx_epit_update_count(s);

        uint64_t next;

        if (tmp > s->cmp) {

            

            next = tmp - s->cmp;

        } else { 

            next = tmp - s->cmp + ((s->cr & CR_RLD) ? TIMER_MAX : s->lr);

        }

        ptimer_set_count(s->timer_cmp, next);

    }

}