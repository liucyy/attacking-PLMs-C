static void integratorcm_write(void *opaque, target_phys_addr_t offset,

                               uint64_t value, unsigned size)

{

    integratorcm_state *s = (integratorcm_state *)opaque;

    switch (offset >> 2) {

    case 2: 

        if (s->cm_lock == 0xa05f)

            s->cm_osc = value;

        break;

    case 3: 

        integratorcm_set_ctrl(s, value);

        break;

    case 5: 

        s->cm_lock = value & 0xffff;

        break;

    case 7: 

        if (s->cm_lock == 0xa05f)

            s->cm_auxosc = value;

        break;

    case 8: 

        s->cm_sdram = value;

        break;

    case 9: 

        

        s->cm_init = value;

        break;

    case 12: 

        s->cm_flags |= value;

        break;

    case 13: 

        s->cm_flags &= ~value;

        break;

    case 14: 

        s->cm_nvflags |= value;

        break;

    case 15: 

        s->cm_nvflags &= ~value;

        break;

    case 18: 

        s->irq_enabled |= value;

        integratorcm_update(s);

        break;

    case 19: 

        s->irq_enabled &= ~value;

        integratorcm_update(s);

        break;

    case 20: 

        s->int_level |= (value & 1);

        integratorcm_update(s);

        break;

    case 21: 

        s->int_level &= ~(value & 1);

        integratorcm_update(s);

        break;

    case 26: 

        s->fiq_enabled |= value;

        integratorcm_update(s);

        break;

    case 27: 

        s->fiq_enabled &= ~value;

        integratorcm_update(s);

        break;

    case 32: 

    case 33: 

    case 34: 

    case 35: 

        

        break;

    default:

        hw_error("integratorcm_write: Unimplemented offset 0x%x\n",

                 (int)offset);

        break;

    }

}