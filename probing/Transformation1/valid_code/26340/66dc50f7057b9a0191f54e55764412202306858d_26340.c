static int sch_handle_start_func_passthrough(SubchDev *sch)

{



    PMCW *p = &sch->curr_status.pmcw;

    SCSW *s = &sch->curr_status.scsw;

    int ret;



    ORB *orb = &sch->orb;

    if (!(s->ctrl & SCSW_ACTL_SUSP)) {

        assert(orb != NULL);

        p->intparm = orb->intparm;

    }



    

    if (!(orb->ctrl0 & ORB_CTRL0_MASK_PFCH) ||

        !(orb->ctrl0 & ORB_CTRL0_MASK_C64)) {

        return -EINVAL;

    }



    ret = s390_ccw_cmd_request(orb, s, sch->driver_data);

    switch (ret) {

    

    case 0:

        break;

    case -EBUSY:

        break;

    case -ENODEV:

        break;

    case -EACCES:

        

        ret = -ENODEV;

        break;

    default:

       

       break;

    };



    return ret;

}