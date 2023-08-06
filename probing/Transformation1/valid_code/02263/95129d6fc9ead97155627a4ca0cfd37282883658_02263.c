static int virtio_net_load_device(VirtIODevice *vdev, QEMUFile *f,

                                  int version_id)

{

    VirtIONet *n = VIRTIO_NET(vdev);

    int i, link_down;



    qemu_get_buffer(f, n->mac, ETH_ALEN);

    n->vqs[0].tx_waiting = qemu_get_be32(f);



    virtio_net_set_mrg_rx_bufs(n, qemu_get_be32(f),

                               virtio_has_feature(vdev, VIRTIO_F_VERSION_1));



    if (version_id >= 3)

        n->status = qemu_get_be16(f);



    if (version_id >= 4) {

        if (version_id < 8) {

            n->promisc = qemu_get_be32(f);

            n->allmulti = qemu_get_be32(f);

        } else {

            n->promisc = qemu_get_byte(f);

            n->allmulti = qemu_get_byte(f);

        }

    }



    if (version_id >= 5) {

        n->mac_table.in_use = qemu_get_be32(f);

        

        if (n->mac_table.in_use <= MAC_TABLE_ENTRIES) {

            qemu_get_buffer(f, n->mac_table.macs,

                            n->mac_table.in_use * ETH_ALEN);

        } else {

            int64_t i;



            

            for (i = 0; i < (int64_t)n->mac_table.in_use * ETH_ALEN; ++i) {

                qemu_get_byte(f);

            }

            n->mac_table.multi_overflow = n->mac_table.uni_overflow = 1;

            n->mac_table.in_use = 0;

        }

    }

 

    if (version_id >= 6)

        qemu_get_buffer(f, (uint8_t *)n->vlans, MAX_VLAN >> 3);



    if (version_id >= 7) {

        if (qemu_get_be32(f) && !peer_has_vnet_hdr(n)) {

            error_report("virtio-net: saved image requires vnet_hdr=on");

            return -1;

        }

    }



    if (version_id >= 9) {

        n->mac_table.multi_overflow = qemu_get_byte(f);

        n->mac_table.uni_overflow = qemu_get_byte(f);

    }



    if (version_id >= 10) {

        n->alluni = qemu_get_byte(f);

        n->nomulti = qemu_get_byte(f);

        n->nouni = qemu_get_byte(f);

        n->nobcast = qemu_get_byte(f);

    }



    if (version_id >= 11) {

        if (qemu_get_byte(f) && !peer_has_ufo(n)) {

            error_report("virtio-net: saved image requires TUN_F_UFO support");

            return -1;

        }

    }



    if (n->max_queues > 1) {

        if (n->max_queues != qemu_get_be16(f)) {

            error_report("virtio-net: different max_queues ");

            return -1;

        }



        n->curr_queues = qemu_get_be16(f);

        if (n->curr_queues > n->max_queues) {

            error_report("virtio-net: curr_queues %x > max_queues %x",

                         n->curr_queues, n->max_queues);

            return -1;

        }

        for (i = 1; i < n->curr_queues; i++) {

            n->vqs[i].tx_waiting = qemu_get_be32(f);

        }

    }



    if (virtio_has_feature(vdev, VIRTIO_NET_F_CTRL_GUEST_OFFLOADS)) {

        n->curr_guest_offloads = qemu_get_be64(f);

    } else {

        n->curr_guest_offloads = virtio_net_supported_guest_offloads(n);

    }



    if (peer_has_vnet_hdr(n)) {

        virtio_net_apply_guest_offloads(n);

    }



    virtio_net_set_queues(n);



    

    for (i = 0; i < n->mac_table.in_use; i++) {

        if (n->mac_table.macs[i * ETH_ALEN] & 1) {

            break;

        }

    }

    n->mac_table.first_multi = i;



    

    link_down = (n->status & VIRTIO_NET_S_LINK_UP) == 0;

    for (i = 0; i < n->max_queues; i++) {

        qemu_get_subqueue(n->nic, i)->link_down = link_down;

    }



    if (virtio_has_feature(vdev, VIRTIO_NET_F_GUEST_ANNOUNCE) &&

        virtio_has_feature(vdev, VIRTIO_NET_F_CTRL_VQ)) {

        n->announce_counter = SELF_ANNOUNCE_ROUNDS;

        timer_mod(n->announce_timer, qemu_clock_get_ms(QEMU_CLOCK_VIRTUAL));

    }



    return 0;

}