void virtio_queue_set_align(VirtIODevice *vdev, int n, int align)

{

    BusState *qbus = qdev_get_parent_bus(DEVICE(vdev));

    VirtioBusClass *k = VIRTIO_BUS_GET_CLASS(qbus);



    

    if (virtio_has_feature(vdev, VIRTIO_F_VERSION_1)) {

        error_report("tried to modify queue alignment for virtio-1 device");

        return;

    }

    

    assert(k->has_variable_vring_alignment);



    vdev->vq[n].vring.align = align;

    virtio_queue_update_rings(vdev, n);

}