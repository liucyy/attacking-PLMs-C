int virtio_blk_handle_scsi_req(VirtIOBlock *blk,

                               VirtQueueElement *elem)

{

    int status = VIRTIO_BLK_S_OK;

    struct virtio_scsi_inhdr *scsi = NULL;

    VirtIODevice *vdev = VIRTIO_DEVICE(blk);



#ifdef __linux__

    int i;

    struct sg_io_hdr hdr;

#endif



    

    if (elem->out_num < 2 || elem->in_num < 3) {

        status = VIRTIO_BLK_S_IOERR;

        goto fail;

    }



    

    scsi = (void *)elem->in_sg[elem->in_num - 2].iov_base;



    if (!blk->conf.scsi) {

        status = VIRTIO_BLK_S_UNSUPP;

        goto fail;

    }



    

    if (elem->out_num > 2 && elem->in_num > 3) {

        status = VIRTIO_BLK_S_UNSUPP;

        goto fail;

    }



#ifdef __linux__

    memset(&hdr, 0, sizeof(struct sg_io_hdr));

    hdr.interface_id = 'S';

    hdr.cmd_len = elem->out_sg[1].iov_len;

    hdr.cmdp = elem->out_sg[1].iov_base;

    hdr.dxfer_len = 0;



    if (elem->out_num > 2) {

        

        hdr.dxfer_direction = SG_DXFER_TO_DEV;

        hdr.iovec_count = elem->out_num - 2;



        for (i = 0; i < hdr.iovec_count; i++)

            hdr.dxfer_len += elem->out_sg[i + 2].iov_len;



        hdr.dxferp = elem->out_sg + 2;



    } else if (elem->in_num > 3) {

        

        hdr.dxfer_direction = SG_DXFER_FROM_DEV;

        hdr.iovec_count = elem->in_num - 3;

        for (i = 0; i < hdr.iovec_count; i++)

            hdr.dxfer_len += elem->in_sg[i].iov_len;



        hdr.dxferp = elem->in_sg;

    } else {

        

        hdr.dxfer_direction = SG_DXFER_NONE;

    }



    hdr.sbp = elem->in_sg[elem->in_num - 3].iov_base;

    hdr.mx_sb_len = elem->in_sg[elem->in_num - 3].iov_len;



    status = bdrv_ioctl(blk->bs, SG_IO, &hdr);

    if (status) {

        status = VIRTIO_BLK_S_UNSUPP;

        goto fail;

    }



    

    if (hdr.status == 0 && hdr.sb_len_wr > 0) {

        hdr.status = CHECK_CONDITION;

    }



    virtio_stl_p(vdev, &scsi->errors,

                 hdr.status | (hdr.msg_status << 8) |

                 (hdr.host_status << 16) | (hdr.driver_status << 24));

    virtio_stl_p(vdev, &scsi->residual, hdr.resid);

    virtio_stl_p(vdev, &scsi->sense_len, hdr.sb_len_wr);

    virtio_stl_p(vdev, &scsi->data_len, hdr.dxfer_len);



    return status;

#else

    abort();

#endif



fail:

    

    if (scsi) {

        virtio_stl_p(vdev, &scsi->errors, 255);

    }

    return status;

}