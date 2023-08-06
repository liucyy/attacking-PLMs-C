int bdrv_aio_multiwrite(BlockDriverState *bs, BlockRequest *reqs, int num_reqs)

{

    MultiwriteCB *mcb;

    int i;



    

    if (bs->drv == NULL) {

        for (i = 0; i < num_reqs; i++) {

            reqs[i].error = -ENOMEDIUM;

        }

        return -1;

    }



    if (num_reqs == 0) {

        return 0;

    }



    

    mcb = g_malloc0(sizeof(*mcb) + num_reqs * sizeof(*mcb->callbacks));

    mcb->num_requests = 0;

    mcb->num_callbacks = num_reqs;



    for (i = 0; i < num_reqs; i++) {

        mcb->callbacks[i].cb = reqs[i].cb;

        mcb->callbacks[i].opaque = reqs[i].opaque;

    }



    

    num_reqs = multiwrite_merge(bs, reqs, num_reqs, mcb);



    trace_bdrv_aio_multiwrite(mcb, mcb->num_callbacks, num_reqs);



    

    mcb->num_requests = num_reqs;

    for (i = 0; i < num_reqs; i++) {

        bdrv_co_aio_rw_vector(bs, reqs[i].sector, reqs[i].qiov,

                              reqs[i].nb_sectors, reqs[i].flags,

                              multiwrite_cb, mcb,

                              true);

    }



    return 0;

}