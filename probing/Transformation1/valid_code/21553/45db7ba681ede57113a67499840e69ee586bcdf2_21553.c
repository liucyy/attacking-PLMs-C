static MemTxResult nvic_sysreg_read(void *opaque, hwaddr addr,

                                    uint64_t *data, unsigned size,

                                    MemTxAttrs attrs)

{

    NVICState *s = (NVICState *)opaque;

    uint32_t offset = addr;

    unsigned i, startvec, end;

    uint32_t val;



    if (attrs.user && !nvic_user_access_ok(s, addr)) {

        

        return MEMTX_ERROR;

    }



    switch (offset) {

    

    case 0x100 ... 0x13f: 

        offset += 0x80;

        

    case 0x180 ... 0x1bf: 

        val = 0;

        startvec = offset - 0x180 + NVIC_FIRST_IRQ; 



        for (i = 0, end = size * 8; i < end && startvec + i < s->num_irq; i++) {

            if (s->vectors[startvec + i].enabled) {

                val |= (1 << i);

            }

        }

        break;

    case 0x200 ... 0x23f: 

        offset += 0x80;

        

    case 0x280 ... 0x2bf: 

        val = 0;

        startvec = offset - 0x280 + NVIC_FIRST_IRQ; 

        for (i = 0, end = size * 8; i < end && startvec + i < s->num_irq; i++) {

            if (s->vectors[startvec + i].pending) {

                val |= (1 << i);

            }

        }

        break;

    case 0x300 ... 0x33f: 

        val = 0;

        startvec = offset - 0x300 + NVIC_FIRST_IRQ; 



        for (i = 0, end = size * 8; i < end && startvec + i < s->num_irq; i++) {

            if (s->vectors[startvec + i].active) {

                val |= (1 << i);

            }

        }

        break;

    case 0x400 ... 0x5ef: 

        val = 0;

        startvec = offset - 0x400 + NVIC_FIRST_IRQ; 



        for (i = 0; i < size && startvec + i < s->num_irq; i++) {

            val |= s->vectors[startvec + i].prio << (8 * i);

        }

        break;

    case 0xd18 ... 0xd23: 

        val = 0;

        for (i = 0; i < size; i++) {

            val |= s->vectors[(offset - 0xd14) + i].prio << (i * 8);

        }

        break;

    case 0xfe0 ... 0xfff: 

        if (offset & 3) {

            val = 0;

        } else {

            val = nvic_id[(offset - 0xfe0) >> 2];

        }

        break;

    default:

        if (size == 4) {

            val = nvic_readl(s, offset);

        } else {

            qemu_log_mask(LOG_GUEST_ERROR,

                          "NVIC: Bad read of size %d at offset 0x%x\n",

                          size, offset);

            val = 0;

        }

    }



    trace_nvic_sysreg_read(addr, val, size);

    *data = val;

    return MEMTX_OK;

}