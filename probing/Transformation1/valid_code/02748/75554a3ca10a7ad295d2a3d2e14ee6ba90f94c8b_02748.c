static inline void omap_gp_timer_trigger(struct omap_gp_timer_s *timer)

{

    if (timer->pt)

        

        omap_gp_timer_out(timer, !timer->out_val);

    else

        

        qemu_irq_pulse(timer->out);

}