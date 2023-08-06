static void virtio_blk_set_status(VirtIODevice *vdev, uint8_t status)

{

    VirtIOBlock *s = VIRTIO_BLK(vdev);

    uint32_t features;



    if (s->dataplane && !(status & (VIRTIO_CONFIG_S_DRIVER |

                                    VIRTIO_CONFIG_S_DRIVER_OK))) {

        virtio_blk_data_plane_stop(s->dataplane);

    }



    if (!(status & VIRTIO_CONFIG_S_DRIVER_OK)) {

        return;

    }



    features = vdev->guest_features;



    

    if (!(features & (1 << VIRTIO_BLK_F_CONFIG_WCE))) {

        aio_context_acquire(bdrv_get_aio_context(s->bs));

        bdrv_set_enable_write_cache(s->bs,

                                    !!(features & (1 << VIRTIO_BLK_F_WCE)));

        aio_context_release(bdrv_get_aio_context(s->bs));

    }

}