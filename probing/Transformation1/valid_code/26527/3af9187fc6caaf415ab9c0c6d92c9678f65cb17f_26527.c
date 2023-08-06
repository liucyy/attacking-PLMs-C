static ssize_t mipsnet_receive(NetClientState *nc, const uint8_t *buf, size_t size)

{

    MIPSnetState *s = qemu_get_nic_opaque(nc);



    trace_mipsnet_receive(size);

    if (!mipsnet_can_receive(nc))







    s->busy = 1;



    



    

    memcpy(s->rx_buffer, buf, size);



    s->rx_count = size;

    s->rx_read = 0;



    

    s->intctl |= MIPSNET_INTCTL_RXDONE;

    mipsnet_update_irq(s);



    return size;