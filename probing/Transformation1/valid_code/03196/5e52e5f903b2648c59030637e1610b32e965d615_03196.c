static void handle_control_message(VirtIOSerial *vser, void *buf, size_t len)

{

    struct VirtIOSerialPort *port;

    struct virtio_console_control cpkt, *gcpkt;

    uint8_t *buffer;

    size_t buffer_len;



    gcpkt = buf;



    if (len < sizeof(cpkt)) {

        

        return;

    }



    cpkt.event = lduw_p(&gcpkt->event);

    cpkt.value = lduw_p(&gcpkt->value);



    port = find_port_by_id(vser, ldl_p(&gcpkt->id));

    if (!port && cpkt.event != VIRTIO_CONSOLE_DEVICE_READY)

        return;



    switch(cpkt.event) {

    case VIRTIO_CONSOLE_DEVICE_READY:

        if (!cpkt.value) {

            error_report("virtio-serial-bus: Guest failure in adding device %s\n",

                         vser->bus->qbus.name);

            break;

        }

        

        QTAILQ_FOREACH(port, &vser->ports, next) {

            send_control_event(port, VIRTIO_CONSOLE_PORT_ADD, 1);

        }

        break;



    case VIRTIO_CONSOLE_PORT_READY:

        if (!cpkt.value) {

            error_report("virtio-serial-bus: Guest failure in adding port %u for device %s\n",

                         port->id, vser->bus->qbus.name);

            break;

        }

        

        if (port->is_console) {

            send_control_event(port, VIRTIO_CONSOLE_CONSOLE_PORT, 1);

        }



        if (port->name) {

            stw_p(&cpkt.event, VIRTIO_CONSOLE_PORT_NAME);

            stw_p(&cpkt.value, 1);



            buffer_len = sizeof(cpkt) + strlen(port->name) + 1;

            buffer = qemu_malloc(buffer_len);



            memcpy(buffer, &cpkt, sizeof(cpkt));

            memcpy(buffer + sizeof(cpkt), port->name, strlen(port->name));

            buffer[buffer_len - 1] = 0;



            send_control_msg(port, buffer, buffer_len);

            qemu_free(buffer);

        }



        if (port->host_connected) {

            send_control_event(port, VIRTIO_CONSOLE_PORT_OPEN, 1);

        }



        

        if (port->info->guest_ready) {

            port->info->guest_ready(port);

        }

        break;



    case VIRTIO_CONSOLE_PORT_OPEN:

        port->guest_connected = cpkt.value;

        if (cpkt.value && port->info->guest_open) {

            

            port->info->guest_open(port);

        }



        if (!cpkt.value && port->info->guest_close) {

            

            port->info->guest_close(port);

        }

        break;

    }

}