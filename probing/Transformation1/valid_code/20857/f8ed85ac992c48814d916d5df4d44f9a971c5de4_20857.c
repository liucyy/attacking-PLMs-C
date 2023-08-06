static void tcx_initfn(Object *obj)

{

    SysBusDevice *sbd = SYS_BUS_DEVICE(obj);

    TCXState *s = TCX(obj);



    memory_region_init_ram(&s->rom, NULL, "tcx.prom", FCODE_MAX_ROM_SIZE,

                           &error_abort);

    memory_region_set_readonly(&s->rom, true);

    sysbus_init_mmio(sbd, &s->rom);



    

    memory_region_init_io(&s->stip, OBJECT(s), &tcx_stip_ops, s, "tcx.stip",

                          TCX_STIP_NREGS);

    sysbus_init_mmio(sbd, &s->stip);



    

    memory_region_init_io(&s->blit, OBJECT(s), &tcx_blit_ops, s, "tcx.blit",

                          TCX_BLIT_NREGS);

    sysbus_init_mmio(sbd, &s->blit);



    

    memory_region_init_io(&s->rstip, OBJECT(s), &tcx_rstip_ops, s, "tcx.rstip",

                          TCX_RSTIP_NREGS);

    sysbus_init_mmio(sbd, &s->rstip);



    

    memory_region_init_io(&s->rblit, OBJECT(s), &tcx_rblit_ops, s, "tcx.rblit",

                          TCX_RBLIT_NREGS);

    sysbus_init_mmio(sbd, &s->rblit);



    

    memory_region_init_io(&s->tec, OBJECT(s), &tcx_dummy_ops, s,

                          "tcx.tec", TCX_TEC_NREGS);

    sysbus_init_mmio(sbd, &s->tec);



    

    memory_region_init_io(&s->dac, OBJECT(s), &tcx_dac_ops, s,

                          "tcx.dac", TCX_DAC_NREGS);

    sysbus_init_mmio(sbd, &s->dac);



    

    memory_region_init_io(&s->thc, OBJECT(s), &tcx_thc_ops, s, "tcx.thc",

                          TCX_THC_NREGS);

    sysbus_init_mmio(sbd, &s->thc);



    

    memory_region_init_io(&s->dhc, OBJECT(s), &tcx_dummy_ops, s, "tcx.dhc",

                          TCX_DHC_NREGS);

    sysbus_init_mmio(sbd, &s->dhc);



    

    memory_region_init_io(&s->alt, OBJECT(s), &tcx_dummy_ops, s, "tcx.alt",

                          TCX_ALT_NREGS);

    sysbus_init_mmio(sbd, &s->alt);



    return;

}