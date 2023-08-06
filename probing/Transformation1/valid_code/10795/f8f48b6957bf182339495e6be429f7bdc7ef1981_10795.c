static void uhci_frame_timer(void *opaque)

{

    UHCIState *s = opaque;



    

    s->expire_time += (get_ticks_per_sec() / FRAME_TIMER_FREQ);

    s->frame_bytes = 0;

    s->completions_only = false;

    qemu_bh_cancel(s->bh);



    if (!(s->cmd & UHCI_CMD_RS)) {

        

        trace_usb_uhci_schedule_stop();

        qemu_del_timer(s->frame_timer);

        uhci_async_cancel_all(s);

        

        s->status |= UHCI_STS_HCHALTED;

        return;

    }



    

    trace_usb_uhci_frame_start(s->frnum);



    uhci_async_validate_begin(s);



    uhci_process_frame(s);



    uhci_async_validate_end(s);



    

    s->frnum = (s->frnum + 1) & 0x7ff;



    

    if (s->pending_int_mask) {

        s->status2 |= s->pending_int_mask;

        s->status  |= UHCI_STS_USBINT;

        uhci_update_irq(s);

    }

    s->pending_int_mask = 0;



    qemu_mod_timer(s->frame_timer, s->expire_time);

}