static void rtc_periodic_timer(void *opaque)

{

    RTCState *s = opaque;



    rtc_timer_update(s, s->next_periodic_time);

#ifdef TARGET_I386

    if ((s->cmos_data[RTC_REG_C] & 0xc0) && rtc_td_hack) {

        s->irq_coalesced++;

        return;

    }

#endif

    if (s->cmos_data[RTC_REG_B] & REG_B_PIE) {

        s->cmos_data[RTC_REG_C] |= 0xc0;

        rtc_irq_raise(s->irq);

    }

    if (s->cmos_data[RTC_REG_B] & REG_B_SQWE) {

        

        qemu_irq_raise(s->sqw_irq);

    }

}