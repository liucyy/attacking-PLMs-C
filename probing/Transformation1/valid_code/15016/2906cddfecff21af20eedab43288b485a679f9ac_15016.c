static void virtio_blk_handle_output(VirtIODevice *vdev, VirtQueue *vq)

{

    VirtIOBlock *s = VIRTIO_BLK(vdev);

    VirtIOBlockReq *req;

    MultiReqBuffer mrb = {};



    

    if (s->dataplane) {

        virtio_blk_data_plane_start(s->dataplane);

        return;

    }



    blk_io_plug(s->blk);



    while ((req = virtio_blk_get_request(s))) {

        virtio_blk_handle_request(req, &mrb);

    }



    if (mrb.num_reqs) {

        virtio_blk_submit_multireq(s->blk, &mrb);

    }



    blk_io_unplug(s->blk);

}