bool virtio_scsi_handle_cmd_req_prepare(VirtIOSCSI *s, VirtIOSCSIReq *req)

{

    VirtIOSCSICommon *vs = &s->parent_obj;

    SCSIDevice *d;

    int rc;



    rc = virtio_scsi_parse_req(req, sizeof(VirtIOSCSICmdReq) + vs->cdb_size,

                               sizeof(VirtIOSCSICmdResp) + vs->sense_size);

    if (rc < 0) {

        if (rc == -ENOTSUP) {

            virtio_scsi_fail_cmd_req(req);

        } else {

            virtio_scsi_bad_req();

        }

        return false;

    }



    d = virtio_scsi_device_find(s, req->req.cmd.lun);

    if (!d) {

        req->resp.cmd.response = VIRTIO_SCSI_S_BAD_TARGET;

        virtio_scsi_complete_cmd_req(req);

        return false;

    }

    if (s->dataplane_started && bdrv_get_aio_context(d->conf.bs) != s->ctx) {

        aio_context_acquire(s->ctx);

        bdrv_set_aio_context(d->conf.bs, s->ctx);

        aio_context_release(s->ctx);

    }

    req->sreq = scsi_req_new(d, req->req.cmd.tag,

                             virtio_scsi_get_lun(req->req.cmd.lun),

                             req->req.cdb, req);



    if (req->sreq->cmd.mode != SCSI_XFER_NONE

        && (req->sreq->cmd.mode != req->mode ||

            req->sreq->cmd.xfer > req->qsgl.size)) {

        req->resp.cmd.response = VIRTIO_SCSI_S_OVERRUN;

        virtio_scsi_complete_cmd_req(req);

        return false;

    }

    scsi_req_ref(req->sreq);

    bdrv_io_plug(d->conf.bs);

    return true;

}