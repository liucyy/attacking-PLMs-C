static struct omap_watchdog_timer_s *omap_wd_timer_init(MemoryRegion *memory,

                hwaddr base,

                qemu_irq irq, omap_clk clk)

{

    struct omap_watchdog_timer_s *s = (struct omap_watchdog_timer_s *)

            g_malloc0(sizeof(struct omap_watchdog_timer_s));



    s->timer.irq = irq;

    s->timer.clk = clk;

    s->timer.timer = timer_new_ns(QEMU_CLOCK_VIRTUAL, omap_timer_tick, &s->timer);

    omap_wd_timer_reset(s);

    omap_timer_clk_setup(&s->timer);



    memory_region_init_io(&s->iomem, NULL, &omap_wd_timer_ops, s,

                          "omap-wd-timer", 0x100);

    memory_region_add_subregion(memory, base, &s->iomem);



    return s;

}