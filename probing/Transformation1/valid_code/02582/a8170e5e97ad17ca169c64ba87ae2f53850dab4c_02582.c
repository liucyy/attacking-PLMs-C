static uint64_t icp_pic_read(void *opaque, target_phys_addr_t offset,

                             unsigned size)

{

    icp_pic_state *s = (icp_pic_state *)opaque;



    switch (offset >> 2) {

    case 0: 

        return s->level & s->irq_enabled;

    case 1: 

        return s->level;

    case 2: 

        return s->irq_enabled;

    case 4: 

        return s->level & 1;

    case 8: 

        return s->level & s->fiq_enabled;

    case 9: 

        return s->level;

    case 10: 

        return s->fiq_enabled;

    case 3: 

    case 5: 

    case 11: 

    default:

        printf ("icp_pic_read: Bad register offset 0x%x\n", (int)offset);

        return 0;

    }

}