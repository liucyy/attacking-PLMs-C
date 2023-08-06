static void armv7m_nvic_reset(DeviceState *dev)

{

    nvic_state *s = NVIC(dev);

    NVICClass *nc = NVIC_GET_CLASS(s);

    nc->parent_reset(dev);

    

    s->gic.cpu_enabled[0] = true;

    s->gic.priority_mask[0] = 0x100;

    

    s->gic.enabled = true;

    systick_reset(s);

}