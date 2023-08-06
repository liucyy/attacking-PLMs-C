static void pchip_write(void *opaque, hwaddr addr,

                        uint64_t v32, unsigned size)

{

    TyphoonState *s = opaque;

    uint64_t val, oldval;



    if (addr & 4) {

        val = v32 << 32 | s->latch_tmp;

        addr ^= 4;

    } else {

        s->latch_tmp = v32;

        return;

    }



    switch (addr) {

    case 0x0000:

        

        s->pchip.win[0].base_addr = val;

        break;

    case 0x0040:

        

        s->pchip.win[1].base_addr = val;

        break;

    case 0x0080:

        

        s->pchip.win[2].base_addr = val;

        break;

    case 0x00c0:

        

        s->pchip.win[3].base_addr = val;

        break;



    case 0x0100:

        

        s->pchip.win[0].mask = val;

        break;

    case 0x0140:

        

        s->pchip.win[1].mask = val;

        break;

    case 0x0180:

        

        s->pchip.win[2].mask = val;

        break;

    case 0x01c0:

        

        s->pchip.win[3].mask = val;

        break;



    case 0x0200:

        

        s->pchip.win[0].translated_base_pfn = val >> 10;

        break;

    case 0x0240:

        

        s->pchip.win[1].translated_base_pfn = val >> 10;

        break;

    case 0x0280:

        

        s->pchip.win[2].translated_base_pfn = val >> 10;

        break;

    case 0x02c0:

        

        s->pchip.win[3].translated_base_pfn = val >> 10;

        break;



    case 0x0300:

        

        oldval = s->pchip.ctl;

        oldval &= ~0x00001cff0fc7ffull;       

        oldval |= val & 0x00001cff0fc7ffull;



        s->pchip.ctl = oldval;

        break;



    case 0x0340:

        

        break;

    case 0x03c0:

        

        break;

    case 0x0400:

        

        break;

    case 0x0440:

        

        break;



    case 0x0480:

        

        break;



    case 0x04c0:

        

        break;



    case 0x0500:

        

    case 0x0540:

        

    case 0x0800:

        

        break;



    default:

        cpu_unassigned_access(current_cpu, addr, true, false, 0, size);

        return;

    }

}