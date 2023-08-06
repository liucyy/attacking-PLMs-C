static void spitz_gpio_setup(PXA2xxState *cpu, int slots)

{

    qemu_irq lcd_hsync;

    

    spitz_hsync = 0;

    lcd_hsync = qemu_allocate_irqs(spitz_lcd_hsync_handler, cpu, 1)[0];

    pxa2xx_gpio_read_notifier(cpu->gpio, lcd_hsync);

    pxa2xx_lcd_vsync_notifier(cpu->lcd, lcd_hsync);



    

    pxa2xx_mmci_handlers(cpu->mmc,

                    qdev_get_gpio_in(cpu->gpio, SPITZ_GPIO_SD_WP),

                    qdev_get_gpio_in(cpu->gpio, SPITZ_GPIO_SD_DETECT));



    

    qemu_irq_raise(qdev_get_gpio_in(cpu->gpio, SPITZ_GPIO_BAT_COVER));



    

    qdev_connect_gpio_out(cpu->gpio, SPITZ_GPIO_ON_RESET, cpu->reset);



    

    if (slots >= 1)

        pxa2xx_pcmcia_set_irq_cb(cpu->pcmcia[0],

                        qdev_get_gpio_in(cpu->gpio, SPITZ_GPIO_CF1_IRQ),

                        qdev_get_gpio_in(cpu->gpio, SPITZ_GPIO_CF1_CD));

    if (slots >= 2)

        pxa2xx_pcmcia_set_irq_cb(cpu->pcmcia[1],

                        qdev_get_gpio_in(cpu->gpio, SPITZ_GPIO_CF2_IRQ),

                        qdev_get_gpio_in(cpu->gpio, SPITZ_GPIO_CF2_CD));

}