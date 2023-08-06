static void mipsnet_receive(void *opaque, const uint8_t *buf, size_t size)

{

    MIPSnetState *s = opaque;



#ifdef DEBUG_MIPSNET_RECEIVE

    printf("mipsnet: receiving len=%d\n", size);

#endif

    if (!mipsnet_can_receive(opaque))

        return;



    s->busy = 1;



    



    

    memcpy(s->rx_buffer, buf, size);



    s->rx_count = size;

    s->rx_read = 0;



    

    s->intctl |= MIPSNET_INTCTL_RXDONE;

    mipsnet_update_irq(s);

}