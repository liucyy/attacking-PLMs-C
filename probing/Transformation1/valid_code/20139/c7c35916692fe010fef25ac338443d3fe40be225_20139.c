static void rtl8139_cplus_transmit(RTL8139State *s)

{

    int txcount = 0;



    while (rtl8139_cplus_transmit_one(s))

    {

        ++txcount;

    }



    

    if (!txcount)

    {

        DPRINTF("C+ mode : transmitter queue stalled, current TxDesc = %d\n",

            s->currCPlusTxDesc);

    }

    else

    {

        

        s->IntrStatus |= TxOK;

        rtl8139_update_irq(s);

    }

}