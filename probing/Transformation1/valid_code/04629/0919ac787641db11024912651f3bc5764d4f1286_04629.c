static void n8x0_i2c_setup(struct n800_s *s)

{

    DeviceState *dev;

    qemu_irq tmp_irq = qdev_get_gpio_in(s->cpu->gpio, N8X0_TMP105_GPIO);



    

    s->i2c = omap_i2c_bus(s->cpu->i2c[0]);



    

    dev = i2c_create_slave(s->i2c, "twl92230", N8X0_MENELAUS_ADDR);

    qdev_connect_gpio_out(dev, 3, s->cpu->irq[0][OMAP_INT_24XX_SYS_NIRQ]);



    

    dev = i2c_create_slave(s->i2c, "tmp105", N8X0_TMP105_ADDR);

    qdev_connect_gpio_out(dev, 0, tmp_irq);

}