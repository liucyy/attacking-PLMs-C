static void i82378_init(DeviceState *dev, I82378State *s)

{

    ISABus *isabus = DO_UPCAST(ISABus, qbus, qdev_get_child_bus(dev, "isa.0"));

    ISADevice *pit;

    qemu_irq *out0_irq;



    



    qdev_init_gpio_out(dev, s->out, 2);

    qdev_init_gpio_in(dev, i82378_request_pic_irq, 16);



    

    out0_irq = qemu_allocate_irqs(i82378_request_out0_irq, s, 1);



    

    s->i8259 = i8259_init(isabus, *out0_irq);

    isa_bus_irqs(isabus, s->i8259);



    

    pit = pit_init(isabus, 0x40, 0, NULL);



    

    pcspk_init(isabus, pit);



    

    DMA_init(1, &s->out[1]);

    isa_create_simple(isabus, "i82374");



    

    isa_create_simple(isabus, "mc146818rtc");

}