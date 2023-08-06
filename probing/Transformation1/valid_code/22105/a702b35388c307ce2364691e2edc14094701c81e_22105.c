static uint32_t slavio_timer_mem_readl(void *opaque, target_phys_addr_t addr)

{

    SLAVIO_TIMERState *s = opaque;

    uint32_t saddr, ret;



    saddr = (addr & TIMER_MAXADDR) >> 2;

    switch (saddr) {

    case 0:

        

        

        if (slavio_timer_is_user(s)) {

            

            slavio_timer_get_out(s);

            ret = s->counthigh;

        } else {

            

            

            qemu_irq_lower(s->irq);

            s->reached = 0;

            ret = s->limit & 0x7fffffff;

        }

        break;

    case 1:

        

        

        slavio_timer_get_out(s);

        if (slavio_timer_is_user(s)) 

            ret = s->count & 0xffffffe00;

        else 

            ret = (s->count & 0x7ffffe00) | s->reached;

        break;

    case 3:

        

        

        ret = s->running;

        break;

    case 4:

        

        

        ret = s->slave_mode;

        break;

    default:

        DPRINTF("invalid read address " TARGET_FMT_plx "\n", addr);

        ret = 0;

        break;

    }

    DPRINTF("read " TARGET_FMT_plx " = %08x\n", addr, ret);



    return ret;

}