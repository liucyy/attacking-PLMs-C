static uint64_t onenand_read(void *opaque, hwaddr addr,

                             unsigned size)

{

    OneNANDState *s = (OneNANDState *) opaque;

    int offset = addr >> s->shift;



    switch (offset) {

    case 0x0000 ... 0xc000:

        return lduw_le_p(s->boot[0] + addr);



    case 0xf000:	

        return s->id.man;

    case 0xf001:	

        return s->id.dev;

    case 0xf002:	

        return s->id.ver;

    

    case 0xf003:	

        return 1 << PAGE_SHIFT;

    case 0xf004:	

        return 0x200;

    case 0xf005:	

        return 1 | (2 << 8);

    case 0xf006:	

        return 0;



    case 0xf100 ... 0xf107:	

        return s->addr[offset - 0xf100];



    case 0xf200:	

        return (s->bufaddr << 8) | ((s->count - 1) & (1 << (PAGE_SHIFT - 10)));



    case 0xf220:	

        return s->command;

    case 0xf221:	

        return s->config[0] & 0xffe0;

    case 0xf222:	

        return s->config[1];



    case 0xf240:	

        return s->status;

    case 0xf241:	

        return s->intstatus;

    case 0xf24c:	

        return s->unladdr[0];

    case 0xf24d:	

        return s->unladdr[1];

    case 0xf24e:	

        return s->wpstatus;



    case 0xff00:	

        return 0x00;

    case 0xff01:	

    case 0xff02:	

    case 0xff03:	

    case 0xff04:	

        hw_error("%s: imeplement ECC\n", __FUNCTION__);

        return 0x0000;

    }



    fprintf(stderr, "%s: unknown OneNAND register %x\n",

                    __FUNCTION__, offset);

    return 0;

}