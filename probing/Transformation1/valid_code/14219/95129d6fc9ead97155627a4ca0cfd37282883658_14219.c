int vring_pop(VirtIODevice *vdev, Vring *vring,

              VirtQueueElement *elem)

{

    struct vring_desc desc;

    unsigned int i, head, found = 0, num = vring->vr.num;

    uint16_t avail_idx, last_avail_idx;

    int ret;



    

    elem->in_num = elem->out_num = 0;



    

    if (vring->broken) {

        ret = -EFAULT;

        goto out;

    }



    

    last_avail_idx = vring->last_avail_idx;

    avail_idx = vring_get_avail_idx(vdev, vring);

    barrier(); 



    if (unlikely((uint16_t)(avail_idx - last_avail_idx) > num)) {

        error_report("Guest moved used index from %u to %u",

                     last_avail_idx, avail_idx);

        ret = -EFAULT;

        goto out;

    }



    

    if (avail_idx == last_avail_idx) {

        ret = -EAGAIN;

        goto out;

    }



    

    smp_rmb();



    

    head = vring_get_avail_ring(vdev, vring, last_avail_idx % num);



    elem->index = head;



    

    if (unlikely(head >= num)) {

        error_report("Guest says index %u > %u is available", head, num);

        ret = -EFAULT;

        goto out;

    }



    i = head;

    do {

        if (unlikely(i >= num)) {

            error_report("Desc index is %u > %u, head = %u", i, num, head);

            ret = -EFAULT;

            goto out;

        }

        if (unlikely(++found > num)) {

            error_report("Loop detected: last one at %u vq size %u head %u",

                         i, num, head);

            ret = -EFAULT;

            goto out;

        }

        copy_in_vring_desc(vdev, &vring->vr.desc[i], &desc);



        

        barrier();



        if (desc.flags & VRING_DESC_F_INDIRECT) {

            ret = get_indirect(vdev, vring, elem, &desc);

            if (ret < 0) {

                goto out;

            }

            continue;

        }



        ret = get_desc(vring, elem, &desc);

        if (ret < 0) {

            goto out;

        }



        i = desc.next;

    } while (desc.flags & VRING_DESC_F_NEXT);



    

    vring->last_avail_idx++;

    if (virtio_has_feature(vdev, VIRTIO_RING_F_EVENT_IDX)) {

        vring_avail_event(&vring->vr) =

            virtio_tswap16(vdev, vring->last_avail_idx);

    }



    return head;



out:

    assert(ret < 0);

    if (ret == -EFAULT) {

        vring->broken = true;

    }

    vring_unmap_element(elem);

    return ret;

}