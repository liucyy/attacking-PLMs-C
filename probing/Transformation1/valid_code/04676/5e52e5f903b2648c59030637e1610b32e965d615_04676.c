VirtIODevice *virtio_serial_init(DeviceState *dev, virtio_serial_conf *conf)

{

    VirtIOSerial *vser;

    VirtIODevice *vdev;

    uint32_t i, max_supported_ports;



    if (!conf->max_virtserial_ports)

        return NULL;



    

    max_supported_ports = VIRTIO_PCI_QUEUE_MAX / 2 - 1;



    if (conf->max_virtserial_ports > max_supported_ports) {

        error_report("maximum ports supported: %u", max_supported_ports);

        return NULL;

    }



    vdev = virtio_common_init("virtio-serial", VIRTIO_ID_CONSOLE,

                              sizeof(struct virtio_console_config),

                              sizeof(VirtIOSerial));



    vser = DO_UPCAST(VirtIOSerial, vdev, vdev);



    

    vser->bus = virtser_bus_new(dev);

    vser->bus->vser = vser;

    QTAILQ_INIT(&vser->ports);



    vser->bus->max_nr_ports = conf->max_virtserial_ports;

    vser->ivqs = qemu_malloc(conf->max_virtserial_ports * sizeof(VirtQueue *));

    vser->ovqs = qemu_malloc(conf->max_virtserial_ports * sizeof(VirtQueue *));



    

    vser->ivqs[0] = virtio_add_queue(vdev, 128, handle_input);

    

    vser->ovqs[0] = virtio_add_queue(vdev, 128, handle_output);



    



    

    vser->c_ivq = virtio_add_queue(vdev, 32, control_in);

    

    vser->c_ovq = virtio_add_queue(vdev, 32, control_out);



    for (i = 1; i < vser->bus->max_nr_ports; i++) {

        

        vser->ivqs[i] = virtio_add_queue(vdev, 128, handle_input);

        

        vser->ovqs[i] = virtio_add_queue(vdev, 128, handle_output);

    }



    vser->config.max_nr_ports = tswap32(conf->max_virtserial_ports);

    vser->ports_map = qemu_mallocz(((conf->max_virtserial_ports + 31) / 32)

        * sizeof(vser->ports_map[0]));

    

    mark_port_added(vser, 0);



    vser->vdev.get_features = get_features;

    vser->vdev.get_config = get_config;

    vser->vdev.set_config = set_config;



    vser->qdev = dev;



    

    register_savevm(dev, "virtio-console", -1, 3, virtio_serial_save,

                    virtio_serial_load, vser);



    return vdev;

}