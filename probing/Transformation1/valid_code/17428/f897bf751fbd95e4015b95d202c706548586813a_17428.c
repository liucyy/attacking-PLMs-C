int vring_pop(VirtIODevice *vdev, Vring *vring,

              VirtQueueElement **p_elem)

{

    struct vring_desc desc;

    unsigned int i, head, found = 0, num = vring->vr.num;

    uint16_t avail_idx, last_avail_idx;

    VirtQueueElement *elem = NULL;

    int ret;



    

    if (vring->broken) {

        ret = -EFAULT;

        goto out;

    }



    

    last_avail_idx = vring->last_avail_idx;

    avail_idx = vring->vr.avail->idx;

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



    

    head = vring->vr.avail->ring[last_avail_idx % num];



    elem = g_slice_new(VirtQueueElement);

    elem->index = head;

    elem->in_num = elem->out_num = 0;

    

    

    if (unlikely(head >= num)) {

        error_report("Guest says index %u > %u is available", head, num);

        ret = -EFAULT;

        goto out;

    }



    if (vdev->guest_features & (1 << VIRTIO_RING_F_EVENT_IDX)) {

        vring_avail_event(&vring->vr) = vring->vr.avail->idx;

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

        desc = vring->vr.desc[i];



        

        barrier();



        if (desc.flags & VRING_DESC_F_INDIRECT) {

            ret = get_indirect(vring, elem, &desc);

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

    *p_elem = elem;

    return head;



out:

    assert(ret < 0);

    if (ret == -EFAULT) {

        vring->broken = true;

    }

    if (elem) {

        vring_unmap_element(elem);

        g_slice_free(VirtQueueElement, elem);

    }

    *p_elem = NULL;

    return ret;

}