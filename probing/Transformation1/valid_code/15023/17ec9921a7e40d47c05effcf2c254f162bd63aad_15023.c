static void chr_event(void *opaque, int event)

{

    Terminal3270 *t = opaque;

    CcwDevice *ccw_dev = CCW_DEVICE(t);

    SubchDev *sch = ccw_dev->sch;



    

    t->in_len = 0;

    t->out_len = 0;

    t->handshake_done = false;

    if (t->timer_tag) {

        g_source_remove(t->timer_tag);

        t->timer_tag = 0;

    }



    switch (event) {

    case CHR_EVENT_OPENED:

        

        t->timer_tag = g_timeout_add_seconds(600, send_timing_mark_cb, t);

        break;

    case CHR_EVENT_CLOSED:

        sch->curr_status.scsw.dstat = SCSW_DSTAT_DEVICE_END;

        css_conditional_io_interrupt(sch);

        break;

    }

}