static uint64_t cchip_read(void *opaque, target_phys_addr_t addr, unsigned size)

{

    CPUAlphaState *env = cpu_single_env;

    TyphoonState *s = opaque;

    uint64_t ret = 0;



    if (addr & 4) {

        return s->latch_tmp;

    }



    switch (addr) {

    case 0x0000:

        

        

        break;



    case 0x0040:

        

        

        break;



    case 0x0080:

        

        ret = s->cchip.misc | (env->cpu_index & 3);

        break;



    case 0x00c0:

        

        break;



    case 0x0100: 

    case 0x0140: 

    case 0x0180: 

    case 0x01c0: 

        

        

        break;



    case 0x0200:

        

        ret = s->cchip.dim[0];

        break;

    case 0x0240:

        

        ret = s->cchip.dim[1];

        break;

    case 0x0280:

        

        ret = s->cchip.dim[0] & s->cchip.drir;

        break;

    case 0x02c0:

        

        ret = s->cchip.dim[1] & s->cchip.drir;

        break;

    case 0x0300:

        

        ret = s->cchip.drir;

        break;



    case 0x0340:

        

        break;



    case 0x0380:

        

        ret = s->cchip.iic[0];

        break;

    case 0x03c0:

        

        ret = s->cchip.iic[1];

        break;



    case 0x0400: 

    case 0x0440: 

    case 0x0480: 

    case 0x04c0: 

        

        break;



    case 0x0580:

        

        

        break;

    case 0x05c0:

        

        break;



    case 0x0600:

        

        ret = s->cchip.dim[2];

        break;

    case 0x0640:

        

        ret = s->cchip.dim[3];

        break;

    case 0x0680:

        

        ret = s->cchip.dim[2] & s->cchip.drir;

        break;

    case 0x06c0:

        

        ret = s->cchip.dim[3] & s->cchip.drir;

        break;



    case 0x0700:

        

        ret = s->cchip.iic[2];

        break;

    case 0x0740:

        

        ret = s->cchip.iic[3];

        break;



    case 0x0780:

        

        break;

    

    case 0x0c00: 

    case 0x0c40: 

    case 0x0c80: 

    case 0x0cc0: 

        break;



    default:

        cpu_unassigned_access(cpu_single_env, addr, 0, 0, 0, size);

        return -1;

    }



    s->latch_tmp = ret >> 32;

    return ret;

}