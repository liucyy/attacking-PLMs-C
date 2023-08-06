static uint32_t virtio_net_bad_features(VirtIODevice *vdev)

{

    uint32_t features = 0;



    

    features |= (1 << VIRTIO_NET_F_MAC);

    features |= (1 << VIRTIO_NET_F_GUEST_CSUM);

    features |= (1 << VIRTIO_NET_F_GUEST_TSO4);

    features |= (1 << VIRTIO_NET_F_GUEST_TSO6);

    features |= (1 << VIRTIO_NET_F_GUEST_ECN);



    return features & virtio_net_get_features(vdev);

}