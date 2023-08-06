static uint64_t a9_scu_read(void *opaque, target_phys_addr_t offset,

                            unsigned size)

{

    a9mp_priv_state *s = (a9mp_priv_state *)opaque;

    switch (offset) {

    case 0x00: 

        return s->scu_control;

    case 0x04: 

        return (((1 << s->num_cpu) - 1) << 4) | (s->num_cpu - 1);

    case 0x08: 

        return s->scu_status;

    case 0x09: 

        return s->scu_status >> 8;

    case 0x0a: 

        return s->scu_status >> 16;

    case 0x0b: 

        return s->scu_status >> 24;

    case 0x0c: 

        return 0;

    case 0x40: 

    case 0x44: 

        

        return 0;

    case 0x50: 

    case 0x54: 

        

    default:

        return 0;

    }

}