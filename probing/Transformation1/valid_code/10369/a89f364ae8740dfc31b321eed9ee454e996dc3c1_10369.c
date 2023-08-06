static void onenand_write(void *opaque, hwaddr addr,

                          uint64_t value, unsigned size)

{

    OneNANDState *s = (OneNANDState *) opaque;

    int offset = addr >> s->shift;

    int sec;



    switch (offset) {

    case 0x0000 ... 0x01ff:

    case 0x8000 ... 0x800f:

        if (s->cycle) {

            s->cycle = 0;



            if (value == 0x0000) {

                SETADDR(ONEN_BUF_BLOCK, ONEN_BUF_PAGE)

                onenand_load_main(s, sec,

                                1 << (PAGE_SHIFT - 9), s->data[0][0]);

                s->addr[ONEN_BUF_PAGE] += 4;

                s->addr[ONEN_BUF_PAGE] &= 0xff;

            }

            break;

        }



        switch (value) {

        case 0x00f0:	

            onenand_reset(s, 0);

            break;



        case 0x00e0:	

            s->cycle = 1;

            break;



        case 0x0090:	

            memset(s->boot[0], 0, 3 << s->shift);

            s->boot[0][0 << s->shift] = s->id.man & 0xff;

            s->boot[0][1 << s->shift] = s->id.dev & 0xff;

            s->boot[0][2 << s->shift] = s->wpstatus & 0xff;

            break;



        default:

            fprintf(stderr, "%s: unknown OneNAND boot command %"PRIx64"\n",

                            __FUNCTION__, value);

        }

        break;



    case 0xf100 ... 0xf107:	

        s->addr[offset - 0xf100] = value;

        break;



    case 0xf200:	

        s->bufaddr = (value >> 8) & 0xf;

        if (PAGE_SHIFT == 11)

            s->count = (value & 3) ?: 4;

        else if (PAGE_SHIFT == 10)

            s->count = (value & 1) ?: 2;

        break;



    case 0xf220:	

        if (s->intstatus & (1 << 15))

            break;

        s->command = value;

        onenand_command(s);

        break;

    case 0xf221:	

        s->config[0] = value;

        onenand_intr_update(s);

        qemu_set_irq(s->rdy, (s->config[0] >> 7) & 1);

        break;

    case 0xf222:	

        s->config[1] = value;

        break;



    case 0xf241:	

        s->intstatus &= value;

        if ((1 << 15) & ~s->intstatus)

            s->status &= ~(ONEN_ERR_CMD | ONEN_ERR_ERASE |

                            ONEN_ERR_PROG | ONEN_ERR_LOAD);

        onenand_intr_update(s);

        break;

    case 0xf24c:	

        s->unladdr[0] = value & (s->blocks - 1);

        

        s->unladdr[1] = value & (s->blocks - 1);

        break;

    case 0xf24d:	

        s->unladdr[1] = value & (s->blocks - 1);

        break;



    default:

        fprintf(stderr, "%s: unknown OneNAND register %x\n",

                        __FUNCTION__, offset);

    }

}