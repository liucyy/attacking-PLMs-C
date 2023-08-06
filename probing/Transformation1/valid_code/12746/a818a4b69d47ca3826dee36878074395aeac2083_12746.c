static int scsi_block_initfn(SCSIDevice *dev)

{

    SCSIDiskState *s = DO_UPCAST(SCSIDiskState, qdev, dev);

    int sg_version;

    int rc;



    if (!s->qdev.conf.bs) {

        error_report("drive property not set");

        return -1;

    }



    

    rc = bdrv_ioctl(s->qdev.conf.bs, SG_GET_VERSION_NUM, &sg_version);

    if (rc < 0) {

        error_report("cannot get SG_IO version number: %s.  "

                     "Is this a SCSI device?",

                     strerror(-rc));

        return -1;

    }

    if (sg_version < 30000) {

        error_report("scsi generic interface too old");

        return -1;

    }



    

    rc = get_device_type(s);

    if (rc < 0) {

        error_report("INQUIRY failed");

        return -1;

    }



    

    if (s->qdev.type == TYPE_ROM || s->qdev.type == TYPE_WORM) {

        s->qdev.blocksize = 2048;

    } else {

        s->qdev.blocksize = 512;

    }



    

    s->features |= (1 << SCSI_DISK_F_NO_REMOVABLE_DEVOPS);



    return scsi_initfn(&s->qdev);

}