static unsigned virtqueue_read_next_desc(VirtIODevice *vdev, VRingDesc *desc,

                                         hwaddr desc_pa, unsigned int max)

{

    unsigned int next;



    

    if (!(desc->flags & VRING_DESC_F_NEXT)) {

        return max;

    }



    

    next = desc->next;

    

    smp_wmb();



    if (next >= max) {

        error_report("Desc next is %u", next);

        exit(1);

    }



    vring_desc_read(vdev, desc, desc_pa, next);

    return next;

}