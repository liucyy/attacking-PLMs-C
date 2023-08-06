static int run_ccw(struct subchannel_id schid, int cmd, void *ptr, int len)

{

    struct ccw1 ccw = {};

    struct cmd_orb orb = {};

    struct schib schib;

    int r;



    

    stsch_err(schid, &schib);

    schib.scsw.ctrl = SCSW_FCTL_START_FUNC;

    msch(schid, &schib);



    

    orb.fmt = 1;

    orb.cpa = (u32)(long)&ccw;

    orb.lpm = 0x80;



    ccw.cmd_code = cmd;

    ccw.cda = (long)ptr;

    ccw.count = len;



    r = ssch(schid, &orb);

    

    drain_irqs(schid);

    return r;

}