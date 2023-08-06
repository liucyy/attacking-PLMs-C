static uint64_t pchip_read(void *opaque, hwaddr addr, unsigned size)

{

    TyphoonState *s = opaque;

    uint64_t ret = 0;



    if (addr & 4) {

        return s->latch_tmp;

    }



    switch (addr) {

    case 0x0000:

        

        ret = s->pchip.win[0].base_addr;

        break;

    case 0x0040:

        

        ret = s->pchip.win[1].base_addr;

        break;

    case 0x0080:

        

        ret = s->pchip.win[2].base_addr;

        break;

    case 0x00c0:

        

        ret = s->pchip.win[3].base_addr;

        break;



    case 0x0100:

        

        ret = s->pchip.win[0].mask;

        break;

    case 0x0140:

        

        ret = s->pchip.win[1].mask;

        break;

    case 0x0180:

        

        ret = s->pchip.win[2].mask;

        break;

    case 0x01c0:

        

        ret = s->pchip.win[3].mask;

        break;



    case 0x0200:

        

        ret = (uint64_t)s->pchip.win[0].translated_base_pfn << 10;

        break;

    case 0x0240:

        

        ret = (uint64_t)s->pchip.win[1].translated_base_pfn << 10;

        break;

    case 0x0280:

        

        ret = (uint64_t)s->pchip.win[2].translated_base_pfn << 10;

        break;

    case 0x02c0:

        

        ret = (uint64_t)s->pchip.win[3].translated_base_pfn << 10;

        break;



    case 0x0300:

        

        ret = s->pchip.ctl;

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

        cpu_unassigned_access(current_cpu, addr, false, false, 0, size);

        return -1;

    }



    s->latch_tmp = ret >> 32;

    return ret;

}