static int virtio_serial_load(QEMUFile *f, void *opaque, int version_id)

{

    VirtIOSerial *s = opaque;

    VirtIOSerialPort *port;

    uint32_t max_nr_ports, nr_active_ports, ports_map;

    unsigned int i;



    if (version_id > 3) {





    

    virtio_load(&s->vdev, f);



    if (version_id < 2) {

        return 0;




    

    qemu_get_be16s(f, &s->config.cols);

    qemu_get_be16s(f, &s->config.rows);



    qemu_get_be32s(f, &max_nr_ports);

    if (max_nr_ports > s->config.max_nr_ports) {

        





    for (i = 0; i < (max_nr_ports + 31) / 32; i++) {

        qemu_get_be32s(f, &ports_map);



        if (ports_map != s->ports_map[i]) {

            






    qemu_get_be32s(f, &nr_active_ports);



    

    for (i = 0; i < nr_active_ports; i++) {

        uint32_t id;

        bool host_connected;



        id = qemu_get_be32(f);

        port = find_port_by_id(s, id);






        port->guest_connected = qemu_get_byte(f);

        host_connected = qemu_get_byte(f);

        if (host_connected != port->host_connected) {

            

            send_control_event(port, VIRTIO_CONSOLE_PORT_OPEN,

                               port->host_connected);




        if (version_id > 2) {

            uint32_t elem_popped;



            qemu_get_be32s(f, &elem_popped);

            if (elem_popped) {

                qemu_get_be32s(f, &port->iov_idx);

                qemu_get_be64s(f, &port->iov_offset);



                qemu_get_buffer(f, (unsigned char *)&port->elem,

                                sizeof(port->elem));

                virtqueue_map_sg(port->elem.in_sg, port->elem.in_addr,

                                 port->elem.in_num, 1);

                virtqueue_map_sg(port->elem.out_sg, port->elem.out_addr,

                                 port->elem.out_num, 1);



                

                virtio_serial_throttle_port(port, false);




    return 0;