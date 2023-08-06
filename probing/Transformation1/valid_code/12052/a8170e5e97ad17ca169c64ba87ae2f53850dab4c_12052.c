static void mpcore_scu_write(void *opaque, target_phys_addr_t offset,

                             uint64_t value, unsigned size)

{

    mpcore_priv_state *s = (mpcore_priv_state *)opaque;

    

    switch (offset) {

    case 0: 

        s->scu_control = value & 1;

        break;

    case 0x0c: 

        

        break;

    default:

        hw_error("mpcore_priv_read: Bad offset %x\n", (int)offset);

    }

}