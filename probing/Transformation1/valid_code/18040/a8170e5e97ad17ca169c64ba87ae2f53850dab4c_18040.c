static void icp_pic_write(void *opaque, target_phys_addr_t offset,

                          uint64_t value, unsigned size)

{

    icp_pic_state *s = (icp_pic_state *)opaque;



    switch (offset >> 2) {

    case 2: 

        s->irq_enabled |= value;

        break;

    case 3: 

        s->irq_enabled &= ~value;

        break;

    case 4: 

        if (value & 1)

            icp_pic_set_irq(s, 0, 1);

        break;

    case 5: 

        if (value & 1)

            icp_pic_set_irq(s, 0, 0);

        break;

    case 10: 

        s->fiq_enabled |= value;

        break;

    case 11: 

        s->fiq_enabled &= ~value;

        break;

    case 0: 

    case 1: 

    case 8: 

    case 9: 

    default:

        printf ("icp_pic_write: Bad register offset 0x%x\n", (int)offset);

        return;

    }

    icp_pic_update(s);

}