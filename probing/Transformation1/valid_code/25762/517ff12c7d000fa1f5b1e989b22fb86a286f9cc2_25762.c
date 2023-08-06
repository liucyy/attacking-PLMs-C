int subch_device_load(SubchDev *s, QEMUFile *f)

{

    SubchDev *old_s;

    Error *err = NULL;

    uint16_t old_schid = s->schid;

    uint16_t old_devno = s->devno;

    int i;



    s->cssid = qemu_get_byte(f);

    s->ssid = qemu_get_byte(f);

    s->schid = qemu_get_be16(f);

    s->devno = qemu_get_be16(f);

    if (s->devno != old_devno) {

        



        error_setg(&err, "%x != %x", old_devno,  s->devno);

        error_append_hint(&err, "Devno mismatch, tried to load wrong section!"

                          " Likely reason: some sequences of plug and unplug"

                          " can break migration for machine versions prior to"

                          " 2.7 (known design flaw).\n");

        error_report_err(err);

        return -EINVAL;

    }

    

    if (old_schid != s->schid) {

        old_s = channel_subsys.css[s->cssid]->sch_set[s->ssid]->sch[old_schid];

        

        if (old_s == s) {

            css_subch_assign(s->cssid, s->ssid, old_schid, s->devno, NULL);

        }

        

        css_subch_assign(s->cssid, s->ssid, s->schid, s->devno, s);

    }

    s->thinint_active = qemu_get_byte(f);

    

    

    s->curr_status.pmcw.intparm = qemu_get_be32(f);

    s->curr_status.pmcw.flags = qemu_get_be16(f);

    s->curr_status.pmcw.devno = qemu_get_be16(f);

    s->curr_status.pmcw.lpm = qemu_get_byte(f);

    s->curr_status.pmcw.pnom  = qemu_get_byte(f);

    s->curr_status.pmcw.lpum = qemu_get_byte(f);

    s->curr_status.pmcw.pim = qemu_get_byte(f);

    s->curr_status.pmcw.mbi = qemu_get_be16(f);

    s->curr_status.pmcw.pom = qemu_get_byte(f);

    s->curr_status.pmcw.pam = qemu_get_byte(f);

    qemu_get_buffer(f, s->curr_status.pmcw.chpid, 8);

    s->curr_status.pmcw.chars = qemu_get_be32(f);

    

    s->curr_status.scsw.flags = qemu_get_be16(f);

    s->curr_status.scsw.ctrl = qemu_get_be16(f);

    s->curr_status.scsw.cpa = qemu_get_be32(f);

    s->curr_status.scsw.dstat = qemu_get_byte(f);

    s->curr_status.scsw.cstat = qemu_get_byte(f);

    s->curr_status.scsw.count = qemu_get_be16(f);

    s->curr_status.mba = qemu_get_be64(f);

    qemu_get_buffer(f, s->curr_status.mda, 4);

    

    qemu_get_buffer(f, s->sense_data, 32);

    s->channel_prog = qemu_get_be64(f);

    

    s->last_cmd.cmd_code = qemu_get_byte(f);

    s->last_cmd.flags = qemu_get_byte(f);

    s->last_cmd.count = qemu_get_be16(f);

    s->last_cmd.cda = qemu_get_be32(f);

    s->last_cmd_valid = qemu_get_byte(f);

    s->id.reserved = qemu_get_byte(f);

    s->id.cu_type = qemu_get_be16(f);

    s->id.cu_model = qemu_get_byte(f);

    s->id.dev_type = qemu_get_be16(f);

    s->id.dev_model = qemu_get_byte(f);

    s->id.unused = qemu_get_byte(f);

    for (i = 0; i < ARRAY_SIZE(s->id.ciw); i++) {

        s->id.ciw[i].type = qemu_get_byte(f);

        s->id.ciw[i].command = qemu_get_byte(f);

        s->id.ciw[i].count = qemu_get_be16(f);

    }

    s->ccw_fmt_1 = qemu_get_byte(f);

    s->ccw_no_data_cnt = qemu_get_byte(f);

    

    if (s->curr_status.pmcw.flags & PMCW_FLAGS_MASK_ENA) {

        if (s->ssid) {

            channel_subsys.max_ssid = MAX_SSID;

        }

        if (s->cssid != channel_subsys.default_cssid) {

            channel_subsys.max_cssid = MAX_CSSID;

        }

    }

    return 0;

}