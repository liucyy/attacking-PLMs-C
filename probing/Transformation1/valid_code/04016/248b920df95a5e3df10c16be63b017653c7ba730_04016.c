static void sch_handle_start_func_virtual(SubchDev *sch)

{



    PMCW *p = &sch->curr_status.pmcw;

    SCSW *s = &sch->curr_status.scsw;

    int path;

    int ret;

    bool suspend_allowed;



    

    path = 0x80;



    if (!(s->ctrl & SCSW_ACTL_SUSP)) {

        

        ORB *orb = &sch->orb;

        s->cstat = 0;

        s->dstat = 0;

        

        p->intparm = orb->intparm;

        if (!(orb->lpm & path)) {

            

            s->flags |= SCSW_FLAGS_MASK_CC;

            s->ctrl &= ~SCSW_CTRL_MASK_STCTL;

            s->ctrl |= (SCSW_STCTL_ALERT | SCSW_STCTL_STATUS_PEND);

            return;

        }

        sch->ccw_fmt_1 = !!(orb->ctrl0 & ORB_CTRL0_MASK_FMT);

        s->flags |= (sch->ccw_fmt_1) ? SCSW_FLAGS_MASK_FMT : 0;

        sch->ccw_no_data_cnt = 0;

        suspend_allowed = !!(orb->ctrl0 & ORB_CTRL0_MASK_SPND);

    } else {

        

        s->ctrl &= ~(SCSW_ACTL_SUSP | SCSW_ACTL_RESUME_PEND);

        

        suspend_allowed = true;

    }

    sch->last_cmd_valid = false;

    do {

        ret = css_interpret_ccw(sch, sch->channel_prog, suspend_allowed);

        switch (ret) {

        case -EAGAIN:

            

            break;

        case 0:

            

            s->ctrl &= ~SCSW_ACTL_START_PEND;

            s->ctrl &= ~SCSW_CTRL_MASK_STCTL;

            s->ctrl |= SCSW_STCTL_PRIMARY | SCSW_STCTL_SECONDARY |

                    SCSW_STCTL_STATUS_PEND;

            s->dstat = SCSW_DSTAT_CHANNEL_END | SCSW_DSTAT_DEVICE_END;

            s->cpa = sch->channel_prog + 8;

            break;

        case -EIO:

            

            break;

        case -ENOSYS:

            

            s->ctrl &= ~SCSW_ACTL_START_PEND;

            s->dstat = SCSW_DSTAT_UNIT_CHECK;

            

            sch->sense_data[0] = 0x80;

            s->ctrl &= ~SCSW_CTRL_MASK_STCTL;

            s->ctrl |= SCSW_STCTL_PRIMARY | SCSW_STCTL_SECONDARY |

                    SCSW_STCTL_ALERT | SCSW_STCTL_STATUS_PEND;

            s->cpa = sch->channel_prog + 8;

            break;

        case -EFAULT:

            

            s->ctrl &= ~SCSW_ACTL_START_PEND;

            s->cstat = SCSW_CSTAT_DATA_CHECK;

            s->ctrl &= ~SCSW_CTRL_MASK_STCTL;

            s->ctrl |= SCSW_STCTL_PRIMARY | SCSW_STCTL_SECONDARY |

                    SCSW_STCTL_ALERT | SCSW_STCTL_STATUS_PEND;

            s->cpa = sch->channel_prog + 8;

            break;

        case -EBUSY:

            

            s->flags &= ~SCSW_FLAGS_MASK_CC;

            s->flags |= (1 << 8);

            s->ctrl &= ~SCSW_CTRL_MASK_STCTL;

            s->ctrl |= SCSW_STCTL_ALERT | SCSW_STCTL_STATUS_PEND;

            break;

        case -EINPROGRESS:

            

            s->ctrl &= ~SCSW_ACTL_START_PEND;

            s->ctrl |= SCSW_ACTL_SUSP;

            break;

        default:

            

            s->ctrl &= ~SCSW_ACTL_START_PEND;

            s->cstat = SCSW_CSTAT_PROG_CHECK;

            s->ctrl &= ~SCSW_CTRL_MASK_STCTL;

            s->ctrl |= SCSW_STCTL_PRIMARY | SCSW_STCTL_SECONDARY |

                    SCSW_STCTL_ALERT | SCSW_STCTL_STATUS_PEND;

            s->cpa = sch->channel_prog + 8;

            break;

        }

    } while (ret == -EAGAIN);



}