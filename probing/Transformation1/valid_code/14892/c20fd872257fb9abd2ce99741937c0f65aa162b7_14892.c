static void virtio_blk_handle_flush(BlockRequest *blkreq, int *num_writes,

    VirtIOBlockReq *req)

{

    BlockDriverAIOCB *acb;



    

    if (*num_writes > 0) {

        do_multiwrite(req->dev->bs, blkreq, *num_writes);

    }

    *num_writes = 0;



    acb = bdrv_aio_flush(req->dev->bs, virtio_blk_flush_complete, req);

    if (!acb) {

        virtio_blk_req_complete(req, VIRTIO_BLK_S_IOERR);

    }

}