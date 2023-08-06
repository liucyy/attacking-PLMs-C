uint32_t ide_data_readl(void *opaque, uint32_t addr)
{
    IDEBus *bus = opaque;
    IDEState *s = idebus_active_if(bus);
    uint8_t *p;
    int ret;
    
    if (!(s->status & DRQ_STAT) || !ide_is_pio_out(s)) {
    p = s->data_ptr;
    ret = cpu_to_le32(*(uint32_t *)p);
    p += 4;
    s->data_ptr = p;
    if (p >= s->data_end)
        s->end_transfer_func(s);
    return ret;