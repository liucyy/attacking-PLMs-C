static void virtio_serial_device_realize(DeviceState *dev, Error **errp)

{

    VirtIODevice *vdev = VIRTIO_DEVICE(dev);

    VirtIOSerial *vser = VIRTIO_SERIAL(dev);

    uint32_t i, max_supported_ports;



    if (!vser->serial.max_virtserial_ports) {

        error_setg(errp, "Maximum number of serial ports not specified");

        return;

    }



    

    max_supported_ports = VIRTIO_QUEUE_MAX / 2 - 1;



    if (vser->serial.max_virtserial_ports > max_supported_ports) {

        error_setg(errp, "maximum ports supported: %u", max_supported_ports);

        return;

    }



    

    

    virtio_init(vdev, "virtio-serial", VIRTIO_ID_CONSOLE,

                offsetof(struct virtio_console_config, emerg_wr));



    

    qbus_create_inplace(&vser->bus, sizeof(vser->bus), TYPE_VIRTIO_SERIAL_BUS,

                        dev, vdev->bus_name);

    qbus_set_hotplug_handler(BUS(&vser->bus), DEVICE(vser), errp);

    vser->bus.vser = vser;

    QTAILQ_INIT(&vser->ports);



    vser->bus.max_nr_ports = vser->serial.max_virtserial_ports;

    vser->ivqs = g_malloc(vser->serial.max_virtserial_ports

                          * sizeof(VirtQueue *));

    vser->ovqs = g_malloc(vser->serial.max_virtserial_ports

                          * sizeof(VirtQueue *));



    

    vser->ivqs[0] = virtio_add_queue(vdev, 128, handle_input);

    

    vser->ovqs[0] = virtio_add_queue(vdev, 128, handle_output);



    



    

    vser->c_ivq = virtio_add_queue(vdev, 32, control_in);

    

    vser->c_ovq = virtio_add_queue(vdev, 32, control_out);



    for (i = 1; i < vser->bus.max_nr_ports; i++) {

        

        vser->ivqs[i] = virtio_add_queue(vdev, 128, handle_input);

        

        vser->ovqs[i] = virtio_add_queue(vdev, 128, handle_output);

    }



    vser->ports_map = g_malloc0(((vser->serial.max_virtserial_ports + 31) / 32)

        * sizeof(vser->ports_map[0]));

    

    mark_port_added(vser, 0);



    vser->post_load = NULL;



    QLIST_INSERT_HEAD(&vserdevices.devices, vser, next);

}