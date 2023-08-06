static void usbredir_device_disconnect(void *priv)

{

    USBRedirDevice *dev = priv;

    int i;



    

    qemu_del_timer(dev->attach_timer);



    if (dev->dev.attached) {

        usb_device_detach(&dev->dev);

        

        dev->next_attach_time = qemu_get_clock_ms(vm_clock) + 200;

    }



    

    usbredir_cleanup_device_queues(dev);

    memset(dev->endpoint, 0, sizeof(dev->endpoint));

    for (i = 0; i < MAX_ENDPOINTS; i++) {

        QTAILQ_INIT(&dev->endpoint[i].bufpq);

    }

    usb_ep_init(&dev->dev);

    dev->interface_info.interface_count = 0;

}