static void a9_scu_write(void *opaque, target_phys_addr_t offset,

                         uint64_t value, unsigned size)

{

    a9mp_priv_state *s = (a9mp_priv_state *)opaque;

    uint32_t mask;

    uint32_t shift;

    switch (size) {

    case 1:

        mask = 0xff;

        break;

    case 2:

        mask = 0xffff;

        break;

    case 4:

        mask = 0xffffffff;

        break;

    default:

        fprintf(stderr, "Invalid size %u in write to a9 scu register %x\n",

                size, (unsigned)offset);

        return;

    }



    switch (offset) {

    case 0x00: 

        s->scu_control = value & 1;

        break;

    case 0x4: 

        break;

    case 0x08: case 0x09: case 0x0A: case 0x0B: 

        shift = (offset - 0x8) * 8;

        s->scu_status &= ~(mask << shift);

        s->scu_status |= ((value & mask) << shift);

        break;

    case 0x0c: 

        

        break;

    case 0x40: 

    case 0x44: 

        

        break;

    case 0x50: 

    case 0x54: 

        

    default:

        break;

    }

}