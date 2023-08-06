static void virtio_net_tx_bh(void *opaque)

{

    VirtIONetQueue *q = opaque;

    VirtIONet *n = q->n;

    VirtIODevice *vdev = VIRTIO_DEVICE(n);

    int32_t ret;



    assert(vdev->vm_running);



    q->tx_waiting = 0;



    

    if (unlikely(!(vdev->status & VIRTIO_CONFIG_S_DRIVER_OK))) {

        return;

    }



    ret = virtio_net_flush_tx(q);

    if (ret == -EBUSY) {

        return; 

    }



    

    if (ret >= n->tx_burst) {

        qemu_bh_schedule(q->tx_bh);

        q->tx_waiting = 1;

        return;

    }



    

    virtio_queue_set_notification(q->tx_vq, 1);

    if (virtio_net_flush_tx(q) > 0) {

        virtio_queue_set_notification(q->tx_vq, 0);

        qemu_bh_schedule(q->tx_bh);

        q->tx_waiting = 1;

    }

}