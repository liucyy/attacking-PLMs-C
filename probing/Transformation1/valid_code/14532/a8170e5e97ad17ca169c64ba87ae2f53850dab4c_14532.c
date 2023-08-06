static uint64_t timerblock_read(void *opaque, target_phys_addr_t addr,

                                unsigned size)

{

    timerblock *tb = (timerblock *)opaque;

    int64_t val;

    switch (addr) {

    case 0: 

        return tb->load;

    case 4: 

        if (((tb->control & 1) == 0) || (tb->count == 0)) {

            return 0;

        }

        

        val = tb->tick - qemu_get_clock_ns(vm_clock);

        val /= timerblock_scale(tb);

        if (val < 0) {

            val = 0;

        }

        return val;

    case 8: 

        return tb->control;

    case 12: 

        return tb->status;

    default:

        return 0;

    }

}