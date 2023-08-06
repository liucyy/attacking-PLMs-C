static void virtio_crypto_dataq_bh(void *opaque)

{

    VirtIOCryptoQueue *q = opaque;

    VirtIOCrypto *vcrypto = q->vcrypto;

    VirtIODevice *vdev = VIRTIO_DEVICE(vcrypto);



    

    if (!vdev->vm_running) {

        return;

    }



    

    if (unlikely(!(vdev->status & VIRTIO_CONFIG_S_DRIVER_OK))) {

        return;

    }



    virtio_crypto_handle_dataq(vdev, q->dataq);

    virtio_queue_set_notification(q->dataq, 1);

}