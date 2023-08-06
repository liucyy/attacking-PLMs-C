static void check_update_timer(RTCState *s)

{

    uint64_t next_update_time;

    uint64_t guest_nsec;

    int next_alarm_sec;



    

    if ((s->cmos_data[RTC_REG_A] & 0x60) == 0x60) {

        timer_del(s->update_timer);

        return;

    }

    if ((s->cmos_data[RTC_REG_C] & REG_C_UF) &&

        (s->cmos_data[RTC_REG_B] & REG_B_SET)) {

        timer_del(s->update_timer);

        return;

    }

    if ((s->cmos_data[RTC_REG_C] & REG_C_UF) &&

        (s->cmos_data[RTC_REG_C] & REG_C_AF)) {

        timer_del(s->update_timer);

        return;

    }



    guest_nsec = get_guest_rtc_ns(s) % NANOSECONDS_PER_SECOND;

    

    next_update_time = qemu_clock_get_ns(rtc_clock)

        + NANOSECONDS_PER_SECOND - guest_nsec;



    

    next_alarm_sec = get_next_alarm(s);

    s->next_alarm_time = next_update_time +

                         (next_alarm_sec - 1) * NANOSECONDS_PER_SECOND;



    if (s->cmos_data[RTC_REG_C] & REG_C_UF) {

        

        next_update_time = s->next_alarm_time;

    }

    if (next_update_time != timer_expire_time_ns(s->update_timer)) {

        timer_mod(s->update_timer, next_update_time);

    }

}