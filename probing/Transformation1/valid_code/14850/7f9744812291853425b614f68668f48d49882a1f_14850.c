static void virtio_net_vmstate_change(void *opaque, int running, int reason)

{

    VirtIONet *n = opaque;

    if (!running) {

        return;

    }

    

    virtio_net_set_status(&n->vdev, n->vdev.status);

}