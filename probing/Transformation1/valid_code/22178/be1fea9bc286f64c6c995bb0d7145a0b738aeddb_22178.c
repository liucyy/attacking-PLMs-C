static int virtqueue_num_heads(VirtQueue *vq, unsigned int idx)

{

    uint16_t num_heads = vring_avail_idx(vq) - idx;



    

    if (num_heads > vq->vring.num) {

        error_report("Guest moved used index from %u to %u",

                     idx, vring_avail_idx(vq));

        exit(1);

    }

    

    if (num_heads) {

        smp_rmb();

    }



    return num_heads;

}