static void hpet_ram_writel(void *opaque, target_phys_addr_t addr,

                            uint32_t value)

{

    int i;

    HPETState *s = opaque;

    uint64_t old_val, new_val, val, index;



    DPRINTF("qemu: Enter hpet_ram_writel at %" PRIx64 " = %#x\n", addr, value);

    index = addr;

    old_val = hpet_ram_readl(opaque, addr);

    new_val = value;



    

    if (index >= 0x100 && index <= 0x3ff) {

        uint8_t timer_id = (addr - 0x100) / 0x20;

        HPETTimer *timer = &s->timer[timer_id];



        DPRINTF("qemu: hpet_ram_writel timer_id = %#x \n", timer_id);

        if (timer_id > s->num_timers) {

            DPRINTF("qemu: timer id out of range\n");

            return;

        }

        switch ((addr - 0x100) % 0x20) {

        case HPET_TN_CFG:

            DPRINTF("qemu: hpet_ram_writel HPET_TN_CFG\n");

            if (activating_bit(old_val, new_val, HPET_TN_FSB_ENABLE)) {

                update_irq(timer, 0);

            }

            val = hpet_fixup_reg(new_val, old_val, HPET_TN_CFG_WRITE_MASK);

            timer->config = (timer->config & 0xffffffff00000000ULL) | val;

            if (new_val & HPET_TN_32BIT) {

                timer->cmp = (uint32_t)timer->cmp;

                timer->period = (uint32_t)timer->period;

            }

            if (activating_bit(old_val, new_val, HPET_TN_ENABLE)) {

                hpet_set_timer(timer);

            } else if (deactivating_bit(old_val, new_val, HPET_TN_ENABLE)) {

                hpet_del_timer(timer);

            }

            break;

        case HPET_TN_CFG + 4: 

            DPRINTF("qemu: invalid HPET_TN_CFG+4 write\n");

            break;

        case HPET_TN_CMP: 

            DPRINTF("qemu: hpet_ram_writel HPET_TN_CMP \n");

            if (timer->config & HPET_TN_32BIT) {

                new_val = (uint32_t)new_val;

            }

            if (!timer_is_periodic(timer)

                || (timer->config & HPET_TN_SETVAL)) {

                timer->cmp = (timer->cmp & 0xffffffff00000000ULL) | new_val;

            }

            if (timer_is_periodic(timer)) {

                

                new_val &= (timer->config & HPET_TN_32BIT ? ~0u : ~0ull) >> 1;

                timer->period =

                    (timer->period & 0xffffffff00000000ULL) | new_val;

            }

            timer->config &= ~HPET_TN_SETVAL;

            if (hpet_enabled(s)) {

                hpet_set_timer(timer);

            }

            break;

        case HPET_TN_CMP + 4: 

            DPRINTF("qemu: hpet_ram_writel HPET_TN_CMP + 4\n");

            if (!timer_is_periodic(timer)

                || (timer->config & HPET_TN_SETVAL)) {

                timer->cmp = (timer->cmp & 0xffffffffULL) | new_val << 32;

            } else {

                

                new_val &= (timer->config & HPET_TN_32BIT ? ~0u : ~0ull) >> 1;

                timer->period =

                    (timer->period & 0xffffffffULL) | new_val << 32;

                }

                timer->config &= ~HPET_TN_SETVAL;

                if (hpet_enabled(s)) {

                    hpet_set_timer(timer);

                }

                break;

        case HPET_TN_ROUTE:

            timer->fsb = (timer->fsb & 0xffffffff00000000ULL) | new_val;

            break;

        case HPET_TN_ROUTE + 4:

            timer->fsb = (new_val << 32) | (timer->fsb & 0xffffffff);

            break;

        default:

            DPRINTF("qemu: invalid hpet_ram_writel\n");

            break;

        }

        return;

    } else {

        switch (index) {

        case HPET_ID:

            return;

        case HPET_CFG:

            val = hpet_fixup_reg(new_val, old_val, HPET_CFG_WRITE_MASK);

            s->config = (s->config & 0xffffffff00000000ULL) | val;

            if (activating_bit(old_val, new_val, HPET_CFG_ENABLE)) {

                

                s->hpet_offset =

                    ticks_to_ns(s->hpet_counter) - qemu_get_clock_ns(vm_clock);

                for (i = 0; i < s->num_timers; i++) {

                    if ((&s->timer[i])->cmp != ~0ULL) {

                        hpet_set_timer(&s->timer[i]);

                    }

                }

            } else if (deactivating_bit(old_val, new_val, HPET_CFG_ENABLE)) {

                

                s->hpet_counter = hpet_get_ticks(s);

                for (i = 0; i < s->num_timers; i++) {

                    hpet_del_timer(&s->timer[i]);

                }

            }

            

            if (activating_bit(old_val, new_val, HPET_CFG_LEGACY)) {

                hpet_pit_disable();

                qemu_irq_lower(s->irqs[RTC_ISA_IRQ]);

            } else if (deactivating_bit(old_val, new_val, HPET_CFG_LEGACY)) {

                hpet_pit_enable();

                qemu_set_irq(s->irqs[RTC_ISA_IRQ], s->rtc_irq_level);

            }

            break;

        case HPET_CFG + 4:

            DPRINTF("qemu: invalid HPET_CFG+4 write \n");

            break;

        case HPET_STATUS:

            val = new_val & s->isr;

            for (i = 0; i < s->num_timers; i++) {

                if (val & (1 << i)) {

                    update_irq(&s->timer[i], 0);

                }

            }

            break;

        case HPET_COUNTER:

            if (hpet_enabled(s)) {

                DPRINTF("qemu: Writing counter while HPET enabled!\n");

            }

            s->hpet_counter =

                (s->hpet_counter & 0xffffffff00000000ULL) | value;

            DPRINTF("qemu: HPET counter written. ctr = %#x -> %" PRIx64 "\n",

                    value, s->hpet_counter);

            break;

        case HPET_COUNTER + 4:

            if (hpet_enabled(s)) {

                DPRINTF("qemu: Writing counter while HPET enabled!\n");

            }

            s->hpet_counter =

                (s->hpet_counter & 0xffffffffULL) | (((uint64_t)value) << 32);

            DPRINTF("qemu: HPET counter + 4 written. ctr = %#x -> %" PRIx64 "\n",

                    value, s->hpet_counter);

            break;

        default:

            DPRINTF("qemu: invalid hpet_ram_writel\n");

            break;

        }

    }

}