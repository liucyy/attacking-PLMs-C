static uint64_t omap_rtc_read(void *opaque, target_phys_addr_t addr,

                              unsigned size)

{

    struct omap_rtc_s *s = (struct omap_rtc_s *) opaque;

    int offset = addr & OMAP_MPUI_REG_MASK;

    uint8_t i;



    if (size != 1) {

        return omap_badwidth_read8(opaque, addr);

    }



    switch (offset) {

    case 0x00:	

        return to_bcd(s->current_tm.tm_sec);



    case 0x04:	

        return to_bcd(s->current_tm.tm_min);



    case 0x08:	

        if (s->pm_am)

            return ((s->current_tm.tm_hour > 11) << 7) |

                    to_bcd(((s->current_tm.tm_hour - 1) % 12) + 1);

        else

            return to_bcd(s->current_tm.tm_hour);



    case 0x0c:	

        return to_bcd(s->current_tm.tm_mday);



    case 0x10:	

        return to_bcd(s->current_tm.tm_mon + 1);



    case 0x14:	

        return to_bcd(s->current_tm.tm_year % 100);



    case 0x18:	

        return s->current_tm.tm_wday;



    case 0x20:	

        return to_bcd(s->alarm_tm.tm_sec);



    case 0x24:	

        return to_bcd(s->alarm_tm.tm_min);



    case 0x28:	

        if (s->pm_am)

            return ((s->alarm_tm.tm_hour > 11) << 7) |

                    to_bcd(((s->alarm_tm.tm_hour - 1) % 12) + 1);

        else

            return to_bcd(s->alarm_tm.tm_hour);



    case 0x2c:	

        return to_bcd(s->alarm_tm.tm_mday);



    case 0x30:	

        return to_bcd(s->alarm_tm.tm_mon + 1);



    case 0x34:	

        return to_bcd(s->alarm_tm.tm_year % 100);



    case 0x40:	

        return (s->pm_am << 3) | (s->auto_comp << 2) |

                (s->round << 1) | s->running;



    case 0x44:	

        i = s->status;

        s->status &= ~0x3d;

        return i;



    case 0x48:	

        return s->interrupts;



    case 0x4c:	

        return ((uint16_t) s->comp_reg) & 0xff;



    case 0x50:	

        return ((uint16_t) s->comp_reg) >> 8;

    }



    OMAP_BAD_REG(addr);

    return 0;

}