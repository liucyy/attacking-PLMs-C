static uint64_t integratorcm_read(void *opaque, target_phys_addr_t offset,

                                  unsigned size)

{

    integratorcm_state *s = (integratorcm_state *)opaque;

    if (offset >= 0x100 && offset < 0x200) {

        

        if (offset >= 0x180)

            return 0;

        return integrator_spd[offset >> 2];

    }

    switch (offset >> 2) {

    case 0: 

        return 0x411a3001;

    case 1: 

        return 0;

    case 2: 

        return s->cm_osc;

    case 3: 

        return s->cm_ctrl;

    case 4: 

        return 0x00100000;

    case 5: 

        if (s->cm_lock == 0xa05f) {

            return 0x1a05f;

        } else {

            return s->cm_lock;

        }

    case 6: 

        

        hw_error("integratorcm_read: CM_LMBUSCNT");

    case 7: 

        return s->cm_auxosc;

    case 8: 

        return s->cm_sdram;

    case 9: 

        return s->cm_init;

    case 10: 

        

        hw_error("integratorcm_read: CM_REFCT");

    case 12: 

        return s->cm_flags;

    case 14: 

        return s->cm_nvflags;

    case 16: 

        return s->int_level & s->irq_enabled;

    case 17: 

        return s->int_level;

    case 18: 

        return s->irq_enabled;

    case 20: 

        return s->int_level & 1;

    case 24: 

        return s->int_level & s->fiq_enabled;

    case 25: 

        return s->int_level;

    case 26: 

        return s->fiq_enabled;

    case 32: 

    case 33: 

    case 34: 

    case 35: 

        

        return 0;

    default:

        hw_error("integratorcm_read: Unimplemented offset 0x%x\n",

                 (int)offset);

        return 0;

    }

}