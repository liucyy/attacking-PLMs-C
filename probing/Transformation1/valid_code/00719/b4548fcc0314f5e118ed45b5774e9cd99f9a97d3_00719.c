grlib_gptimer_writel(void *opaque, target_phys_addr_t addr, uint32_t value)

{

    GPTimerUnit        *unit = opaque;

    target_phys_addr_t  timer_addr;

    int                 id;



    addr &= 0xff;



    

    switch (addr) {

    case SCALER_OFFSET:

        value &= 0xFFFF; 

        unit->scaler = value;

        trace_grlib_gptimer_writel(-1, "scaler:", unit->scaler);

        return;



    case SCALER_RELOAD_OFFSET:

        value &= 0xFFFF; 

        unit->reload = value;

        trace_grlib_gptimer_writel(-1, "reload:", unit->reload);

        grlib_gptimer_set_scaler(unit, value);

        return;



    case CONFIG_OFFSET:

        

        trace_grlib_gptimer_writel(-1, "config (Read Only):", 0);

        return;



    default:

        break;

    }



    timer_addr = (addr % TIMER_BASE);

    id         = (addr - TIMER_BASE) / TIMER_BASE;



    if (id >= 0 && id < unit->nr_timers) {



        

        switch (timer_addr) {

        case COUNTER_OFFSET:

            trace_grlib_gptimer_writel(id, "counter:", value);

            unit->timers[id].counter = value;

            grlib_gptimer_enable(&unit->timers[id]);

            return;



        case COUNTER_RELOAD_OFFSET:

            trace_grlib_gptimer_writel(id, "reload:", value);

            unit->timers[id].reload = value;

            return;



        case CONFIG_OFFSET:

            trace_grlib_gptimer_writel(id, "config:", value);



            if (value & GPTIMER_INT_PENDING) {

                

                value &= ~GPTIMER_INT_PENDING;

            } else {

                

                value |= unit->timers[id].config & GPTIMER_INT_PENDING;

            }



            unit->timers[id].config = value;



            



            if (value & GPTIMER_LOAD) {

                grlib_gptimer_restart(&unit->timers[id]);

            } else if (value & GPTIMER_ENABLE) {

                grlib_gptimer_enable(&unit->timers[id]);

            }



            

            value &= ~(GPTIMER_LOAD & GPTIMER_DEBUG_HALT);



            unit->timers[id].config = value;

            return;



        default:

            break;

        }



    }



    trace_grlib_gptimer_unknown_register("write", addr);

}