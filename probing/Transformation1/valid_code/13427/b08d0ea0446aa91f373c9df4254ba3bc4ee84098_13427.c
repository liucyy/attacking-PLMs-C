static SCSIRequest *scsi_block_new_request(SCSIDevice *d, uint32_t tag,

                                           uint32_t lun, uint8_t *buf,

                                           void *hba_private)

{

    SCSIDiskState *s = DO_UPCAST(SCSIDiskState, qdev, d);



    switch (buf[0]) {

    case READ_6:

    case READ_10:

    case READ_12:

    case READ_16:

    case VERIFY_10:

    case VERIFY_12:

    case VERIFY_16:

    case WRITE_6:

    case WRITE_10:

    case WRITE_12:

    case WRITE_16:

    case WRITE_VERIFY_10:

    case WRITE_VERIFY_12:

    case WRITE_VERIFY_16:

        

        if (bdrv_get_flags(s->qdev.conf.bs) & BDRV_O_NOCACHE) {

            break;

        }



        

        if (s->qdev.type == TYPE_ROM) {

            break;

	}

        return scsi_req_alloc(&scsi_disk_reqops, &s->qdev, tag, lun,

                              hba_private);

    }



    return scsi_req_alloc(&scsi_generic_req_ops, &s->qdev, tag, lun,

                          hba_private);

}