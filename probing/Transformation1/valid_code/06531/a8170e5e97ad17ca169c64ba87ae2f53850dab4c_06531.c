static void omap_ulpd_pm_write(void *opaque, target_phys_addr_t addr,

                               uint64_t value, unsigned size)

{

    struct omap_mpu_state_s *s = (struct omap_mpu_state_s *) opaque;

    int64_t now, ticks;

    int div, mult;

    static const int bypass_div[4] = { 1, 2, 4, 4 };

    uint16_t diff;



    if (size != 2) {

        return omap_badwidth_write16(opaque, addr, value);

    }



    switch (addr) {

    case 0x00:	

    case 0x04:	

    case 0x08:	

    case 0x0c:	

    case 0x14:	

    case 0x40:	

        OMAP_RO_REG(addr);

        break;



    case 0x10:	

        

        if ((s->ulpd_pm_regs[addr >> 2] ^ value) & 1) {

            now = qemu_get_clock_ns(vm_clock);



            if (value & 1)

                s->ulpd_gauge_start = now;

            else {

                now -= s->ulpd_gauge_start;



                

                ticks = muldiv64(now, 32768, get_ticks_per_sec());

                s->ulpd_pm_regs[0x00 >> 2] = (ticks >>  0) & 0xffff;

                s->ulpd_pm_regs[0x04 >> 2] = (ticks >> 16) & 0xffff;

                if (ticks >> 32)	

                    s->ulpd_pm_regs[0x14 >> 2] |= 1 << 2;



                

                ticks = muldiv64(now, 12000000, get_ticks_per_sec());

                s->ulpd_pm_regs[0x08 >> 2] = (ticks >>  0) & 0xffff;

                s->ulpd_pm_regs[0x0c >> 2] = (ticks >> 16) & 0xffff;

                if (ticks >> 32)	

                    s->ulpd_pm_regs[0x14 >> 2] |= 1 << 1;



                s->ulpd_pm_regs[0x14 >> 2] |= 1 << 0;	

                qemu_irq_raise(qdev_get_gpio_in(s->ih[1], OMAP_INT_GAUGE_32K));

            }

        }

        s->ulpd_pm_regs[addr >> 2] = value;

        break;



    case 0x18:	

    case 0x1c:	

    case 0x20:	

    case 0x28:	

    case 0x2c:	

        OMAP_BAD_REG(addr);

    case 0x24:	

    case 0x38:	

    case 0x48:	

    case 0x50:	

        s->ulpd_pm_regs[addr >> 2] = value;

        break;



    case 0x30:	

        diff = s->ulpd_pm_regs[addr >> 2] ^ value;

        s->ulpd_pm_regs[addr >> 2] = value & 0x3f;

        omap_ulpd_clk_update(s, diff, value);

        break;



    case 0x34:	

        diff = s->ulpd_pm_regs[addr >> 2] ^ value;

        s->ulpd_pm_regs[addr >> 2] = value & 0x1f;

        omap_ulpd_req_update(s, diff, value);

        break;



    case 0x3c:	

        

        

        diff = s->ulpd_pm_regs[addr >> 2] & value;

        s->ulpd_pm_regs[addr >> 2] = value & 0x2fff;

        if (diff & (0x3ff << 2)) {

            if (value & (1 << 4)) {			

                div = ((value >> 5) & 3) + 1;		

                mult = MIN((value >> 7) & 0x1f, 1);	

            } else {

                div = bypass_div[((value >> 2) & 3)];	

                mult = 1;

            }

            omap_clk_setrate(omap_findclk(s, "dpll4"), div, mult);

        }



        

        s->ulpd_pm_regs[addr >> 2] =

                (s->ulpd_pm_regs[addr >> 2] & 0xfffe) |

                ((s->ulpd_pm_regs[addr >> 2] >> 4) & 1);



        

        s->ulpd_pm_regs[addr >> 2] |= 2;

        break;



    case 0x4c:	

        diff = s->ulpd_pm_regs[addr >> 2] & value;

        s->ulpd_pm_regs[addr >> 2] = value & 0xf;

        if (diff & (1 << 0))				

            omap_clk_reparent(omap_findclk(s, "ck_48m"), omap_findclk(s,

                                    (value & (1 << 0)) ? "apll" : "dpll4"));

        break;



    default:

        OMAP_BAD_REG(addr);

    }

}