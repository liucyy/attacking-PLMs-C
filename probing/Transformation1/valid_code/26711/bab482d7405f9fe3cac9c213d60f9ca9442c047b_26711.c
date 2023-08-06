static void do_subchannel_work(SubchDev *sch, ORB *orb)

{



    SCSW *s = &sch->curr_status.scsw;



    if (s->ctrl & SCSW_FCTL_CLEAR_FUNC) {

        sch_handle_clear_func(sch);

    } else if (s->ctrl & SCSW_FCTL_HALT_FUNC) {

        sch_handle_halt_func(sch);

    } else if (s->ctrl & SCSW_FCTL_START_FUNC) {

        

        sch_handle_start_func(sch, orb);

    } else {

        

        return;

    }

    css_inject_io_interrupt(sch);

}