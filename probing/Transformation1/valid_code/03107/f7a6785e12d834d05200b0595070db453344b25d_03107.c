static uint64_t imx_serial_read(void *opaque, hwaddr offset,

                                unsigned size)

{

    IMXSerialState *s = (IMXSerialState *)opaque;

    uint32_t c;



    DPRINTF("read(offset=%x)\n", offset >> 2);

    switch (offset >> 2) {

    case 0x0: 

        c = s->readbuff;

        if (!(s->uts1 & UTS1_RXEMPTY)) {

            

            c |= URXD_CHARRDY;

            s->usr1 &= ~USR1_RRDY;

            s->usr2 &= ~USR2_RDR;

            s->uts1 |= UTS1_RXEMPTY;

            imx_update(s);

            qemu_chr_accept_input(s->chr);

        }

        return c;



    case 0x20: 

        return s->ucr1;



    case 0x21: 

        return s->ucr2;



    case 0x25: 

        return s->usr1;



    case 0x26: 

        return s->usr2;



    case 0x2A: 

        return s->ubmr;



    case 0x2B: 

        return s->ubrc;



    case 0x2d: 

        return s->uts1;



    case 0x24: 

        return s->ufcr;



    case 0x2c:

        return s->onems;



    case 0x22: 

        return s->ucr3;



    case 0x23: 

    case 0x29: 

        return 0x0; 



    default:

        IPRINTF("%s: bad offset: 0x%x\n", __func__, (int)offset);

        return 0;

    }

}