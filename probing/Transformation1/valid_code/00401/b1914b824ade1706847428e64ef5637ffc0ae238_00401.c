static int virtio_ccw_set_vqs(SubchDev *sch, VqInfoBlock *info,

                              VqInfoBlockLegacy *linfo)

{

    VirtIODevice *vdev = virtio_ccw_get_vdev(sch);

    uint16_t index = info ? info->index : linfo->index;

    uint16_t num = info ? info->num : linfo->num;

    uint64_t desc = info ? info->desc : linfo->queue;



    if (index >= VIRTIO_CCW_QUEUE_MAX) {

        return -EINVAL;

    }



    

    if (linfo && desc && (linfo->align != 4096)) {

        return -EINVAL;

    }



    if (!vdev) {

        return -EINVAL;

    }



    if (info) {

        virtio_queue_set_rings(vdev, index, desc, info->avail, info->used);

    } else {

        virtio_queue_set_addr(vdev, index, desc);

    }

    if (!desc) {

        virtio_queue_set_vector(vdev, index, VIRTIO_NO_VECTOR);

    } else {

        if (info) {

            

            if (virtio_queue_get_max_num(vdev, index) < num) {

                

                return -EINVAL;

            }

            virtio_queue_set_num(vdev, index, num);

        } else if (virtio_queue_get_num(vdev, index) > num) {

            

            return -EINVAL;

        }

        

        virtio_queue_set_vector(vdev, index, index);

    }

    

    vdev->config_vector = VIRTIO_CCW_QUEUE_MAX;

    return 0;

}